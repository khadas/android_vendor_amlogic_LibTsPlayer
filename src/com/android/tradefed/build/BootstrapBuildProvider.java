/*
 * Copyright (C) 2013 The Android Open Source Project
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

package com.android.tradefed.build;

import com.android.tradefed.config.Option;
import com.android.tradefed.config.OptionClass;
import com.android.tradefed.device.DeviceNotAvailableException;
import com.android.tradefed.device.ITestDevice;
import com.android.tradefed.log.LogUtil.CLog;

/**
 * A {@link IDeviceBuildProvider} that bootstraps build info from the test device
 *
 * <p>
 * This is typically used for devices with an externally supplied build, i.e. not generated by
 * in-house build system. Certain information, specifically the branch, is not actually available
 * from the device, therefore it's artificially generated.
 *
 * <p>All build meta data info comes from various ro.* property fields on device
 *
 * <p>Currently this build provider generates meta data as follows:
 * <ul>
 * <li>branch:
 * $(ro.product.brand)-$(ro.product.name)-$(ro.product.device)-$(ro.build.version.release),
 * for example:
 * <ul>
 *   <li>for Google Play edition Samsung S4 running Android 4.2: samsung-jgedlteue-jgedlte-4.2
 *   <li>for Nexus 7 running Android 4.2: google-nakasi-grouper-4.2
 * </ul>
 * <li>build flavor: as provided by {@link ITestDevice#getBuildFlavor()}
 * <li>build alias: as provided by {@link ITestDevice#getBuildAlias()}
 * <li>build id: as provided by {@link ITestDevice#getBuildId()}
 */
@OptionClass(alias = "bootstrap-build")
public class BootstrapBuildProvider implements IDeviceBuildProvider {

    @Option(name="test-tag", description="test tag name to supply.")
    private String mTestTag = "stub";

    @Option(name="build-target", description="build target name to supply.")
    private String mBuildTargetName = "bootstrapped";

    @Option(name="shell-available-timeout",
            description="Time to wait in seconds for device shell to become available. " +
            "Default to 300 seconds.")
    private long mShellAvailableTimeout = 5 * 60;

    @Override
    public IBuildInfo getBuild() throws BuildRetrievalError {
        throw new UnsupportedOperationException("Call getBuild(ITestDevice)");
    }

    @Override
    public void buildNotTested(IBuildInfo info) {
        // no op
        CLog.i("ignoring buildNotTested call, build = %s ", info.getBuildId());
    }

    @Override
    public void cleanUp(IBuildInfo info) {
        // no op
    }

    @Override
    public IBuildInfo getBuild(ITestDevice device) throws BuildRetrievalError,
            DeviceNotAvailableException {
        IBuildInfo info = new BuildInfo(device.getBuildId(), mTestTag, mBuildTargetName);
        if (!device.waitForDeviceShell(mShellAvailableTimeout * 1000)) {
            throw new DeviceNotAvailableException(
                    String.format("Shell did not become available in %d seconds",
                            mShellAvailableTimeout));
        }
        info.setBuildBranch(String.format("%s-%s-%s-%s",
                device.getProperty("ro.product.brand"),
                device.getProperty("ro.product.name"),
                device.getProductVariant(),
                device.getProperty("ro.build.version.release")));
        info.setBuildFlavor(device.getBuildFlavor());
        info.addBuildAttribute("build_alias", device.getBuildAlias());
        return info;
    }
}