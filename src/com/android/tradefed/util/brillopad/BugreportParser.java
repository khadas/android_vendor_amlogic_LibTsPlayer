/*
 * Copyright (C) 2011 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
package com.android.tradefed.util.brillopad;

import com.android.tradefed.result.InputStreamSource;
import com.android.tradefed.util.RegexTrie;
import com.android.tradefed.util.brillopad.section.AbstractSectionParser;
import com.android.tradefed.util.brillopad.section.MemInfoParser;
import com.android.tradefed.util.brillopad.section.NoopSectionParser;
import com.android.tradefed.util.brillopad.section.ProcRankParser;
import com.android.tradefed.util.brillopad.section.SystemLogParser;
import com.android.tradefed.util.brillopad.section.SystemPropParser;
import com.android.tradefed.util.brillopad.section.syslog.AnrParser;
import com.android.tradefed.util.brillopad.section.syslog.JavaCrashParser;
import com.android.tradefed.util.brillopad.section.syslog.NativeCrashParser;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;

/**
 * A brillopad parser that understands the Android bugreport format
 */
public class BugreportParser extends AbstractSectionParser {
    private static final String MEM_INFO_SECTION_REGEX = "------ MEMORY INFO .*";
    private static final String PROCRANK_SECTION_REGEX = "------ PROCRANK .*";
    private static final String SYSTEM_PROP_SECTION_REGEX = "------ SYSTEM PROPERTIES .*";
    private static final String SYSTEM_LOG_SECTION_REGEX = "------ SYSTEM LOG .*";
    private static final String NOOP_SECTION_REGEX = "------ .*";

    // For convenience, since these will likely be the most common events that people are
    // interested in.
    public static final String ANR = AnrParser.SECTION_NAME;
    public static final String JAVA_CRASH = JavaCrashParser.SECTION_NAME;
    public static final String NATIVE_CRASH = NativeCrashParser.SECTION_NAME;

    /**
     * Parse a bugreport file into an {@link ItemList} object.  This is the main entrance method to
     * the parser.
     *
     * @param input a {@link BufferedReader} from which a bugreport can be read
     * @return an {@link ItemList} containing the parsed elements of the bugreport
     */
    public ItemList parse(BufferedReader input) throws IOException {
        ItemList itemlist = new ItemList();
        String line;
        while ((line = input.readLine()) != null) {
            parseLine(line, itemlist);
        }

        // signal EOF
        commit(itemlist);

        return itemlist;
    }

    /**
     * Helper method to retrieve the number of ANRs that occurred in a parsed bugreport
     */
    public int getAnrCount(ItemList parsedData) {
        return parsedData.getItemsByType(ANR).size();
    }

    /**
     * Helper method to retrieve the number of Java crashes that occurred in a parsed bugreport
     */
    public int getJavaCrashCount(ItemList parsedData) {
        return parsedData.getItemsByType(JAVA_CRASH).size();
    }

    /**
     * Helper method to retrieve the number of native crashes that occurred in a parsed bugreport
     */
    public int getNativeCrashCount(ItemList parsedData) {
        return parsedData.getItemsByType(NATIVE_CRASH).size();
    }

    /**
     * Parse a bugreport InputStreamSource into an {@link ItemList} object.  This is an entrance
     * method that makes {@link BugreportParser} convenient to use from Tradefed-based tests.
     *
     * @param input an {@link InputStreamSource} from which a bugreport can be read
     * @return an {@link ItemList} containing the parsed elements of the bugreport
     */
    public ItemList parse(InputStreamSource input) throws IOException {
        InputStream stream = input.createInputStream();
        return parse(new BufferedReader(new InputStreamReader(stream)));
    }

    @Override
    public void addDefaultSectionParsers(RegexTrie<IBlockParser> sectionTrie) {
        sectionTrie.put(new MemInfoParser(), MEM_INFO_SECTION_REGEX);
        sectionTrie.put(new ProcRankParser(), PROCRANK_SECTION_REGEX);
        sectionTrie.put(new SystemPropParser(), SYSTEM_PROP_SECTION_REGEX);
        sectionTrie.put(new SystemLogParser(), SYSTEM_LOG_SECTION_REGEX);

        // Add a default section parser so that the Trie will commit prior sections
        sectionTrie.put(new NoopSectionParser(), NOOP_SECTION_REGEX);
    }
}

