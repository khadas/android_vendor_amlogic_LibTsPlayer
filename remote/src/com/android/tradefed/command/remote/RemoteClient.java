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
package com.android.tradefed.command.remote;

import com.android.ddmlib.Log;
import com.android.tradefed.command.remote.RemoteOperation.RemoteException;

import org.json.JSONException;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.InetAddress;
import java.net.Socket;
import java.net.UnknownHostException;
import java.util.List;

/**
 * Class for sending remote commands to another TF process.
 * <p/>
 * Currently uses JSON-encoded data sent via sockets.
 */
public class RemoteClient implements IRemoteClient {

    private static final String TAG = RemoteClient.class.getSimpleName();
    private final Socket mSocket;
    private final PrintWriter mWriter;
    private final BufferedReader mReader;

    /**
     * Initialize the {@RemoteClient}, and instruct it to connect to the given port on
     * localhost.
     *
     * @param port the tcp/ip port number
     * @throws IOException
     * @throws UnknownHostException
     */
    RemoteClient(int port) throws UnknownHostException, IOException {
        this(InetAddress.getLocalHost().getHostName(), port);
    }

    /**
     * Initialize the {@RemoteClient}, and instruct it to connect to the given hostname and port.
     *
     * @param hostname to connect to
     * @param port the tcp/ip port number
     * @throws IOException
     * @throws UnknownHostException
     */
    RemoteClient(String hostName, int port) throws UnknownHostException, IOException {
        mSocket = new Socket(hostName, port);
        mWriter = new PrintWriter(mSocket.getOutputStream(), true);
        mReader = new BufferedReader(new InputStreamReader(mSocket.getInputStream()));
    }

    /**
     * Send the given command to the remote TF.
     *
     * @param cmd the {@link RemoteOperation} to send
     * @return true if command was sent and processed successfully by remote TF
     */
    private synchronized boolean sendCommand(RemoteOperation cmd) {
       try {
           mWriter.println(cmd.pack());
           String response = mReader.readLine();
           cmd.unpackResponseFromString(response);
           if (cmd.hasError()) {
               Log.e(TAG, "remote command failed: " + cmd.getErrorMsg());
               return false;
           } else {
               return true;
           }
       } catch (RemoteException e) {
           // TODO: convert to CLog once we have tf-common
          Log.e(TAG, "Failed to send remote command");
          Log.e(TAG, e);
       } catch (IOException e) {
           Log.e(TAG, "Failed to send remote command");
           Log.e(TAG, e);
       } catch (JSONException e) {
           Log.e(TAG, "Failed to parse remote command response");
           Log.e(TAG, e);
       }
       return false;
    }

    /**
     * Helper method to create a {@link RemoteClient} connected to given port
     *
     * @param port the tcp/ip port
     * @return the {@link RemoteClient}
     * @throws UnknownHostException
     * @throws IOException
     */
    public static IRemoteClient connect(int port) throws UnknownHostException, IOException {
        return new RemoteClient(port);
    }

    /**
     * Helper method to create a {@link RemoteClient} connected to given host and port
     *
     * @param hostname the host name
     * @param port the tcp/ip port
     * @return the {@link RemoteClient}
     * @throws UnknownHostException
     * @throws IOException
     */
    public static IRemoteClient connect(String hostname, int port)
        throws UnknownHostException, IOException {
        return new RemoteClient(hostname, port);
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public boolean sendAllocateDevice(String serial) throws IOException {
        return sendCommand(new AllocateDeviceOp(serial));
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public boolean sendFreeDevice(String serial) throws IOException {
        return sendCommand(new FreeDeviceOp(serial));
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public boolean sendAddCommand(long totalTime, String... commandArgs) throws IOException {
        return sendCommand(new AddCommandOp(totalTime, commandArgs));
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public boolean sendClose() throws IOException {
        return sendCommand(new CloseOp());
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public boolean sendHandoverClose(int port) throws IOException {
        return sendCommand(new HandoverCloseOp(port));
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public List<DeviceDescriptor> sendListDevices() {
        ListDevicesOp op = new ListDevicesOp();
        if (sendCommand(op)) {
            return op.getDeviceStateMap();
        }
        return null;
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public synchronized void close() {
        if (mSocket != null) {
            try {
                mSocket.close();
            } catch (IOException e) {
                Log.w(TAG, String.format("exception closing socket: %s", e.toString()));
            }
        }
        if (mWriter != null) {
            mWriter.close();
        }
    }
}