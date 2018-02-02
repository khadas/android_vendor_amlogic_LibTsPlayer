package com.android.tradefed.result;

import com.android.ddmlib.testrunner.TestIdentifier;

import java.util.Map;

/**
 * Receives event notifications during instrumentation test runs.
 *
 * <p>Patterned after org.junit.runner.notification.RunListener
 *
 * <p>The sequence of calls will be:
 *
 * <ul>
 *   <li> testRunStarted
 *   <li> testStarted
 *   <li> [testFailed]
 *   <li> [testAssumptionFailure]
 *   <li> [testIgnored]
 *   <li> testEnded
 *   <li> ....
 *   <li> [testRunFailed]
 *   <li> testRunEnded
 * </ul>
 */
public interface ITestLifeCycleReceiver {

    /**
     * Reports the start of a test run.
     *
     * @param runName the test run name
     * @param testCount total number of tests in test run
     */
    public default void testRunStarted(String runName, int testCount) {}

    /**
     * Reports test run failed to complete due to a fatal error.
     *
     * @param errorMessage {@link String} describing reason for run failure.
     */
    public default void testRunFailed(String errorMessage) {}

    /**
     * Reports end of test run.
     *
     * @param elapsedTimeMillis device reported elapsed time, in milliseconds
     * @param runMetrics key-value pairs reported at the end of a test run
     */
    public default void testRunEnded(long elapsedTimeMillis, Map<String, String> runMetrics) {}

    /**
     * Reports test run stopped before completion due to a user request.
     *
     * <p>TODO: currently unused, consider removing
     *
     * @param elapsedTime device reported elapsed time, in milliseconds
     */
    public default void testRunStopped(long elapsedTime) {}

    /**
     * Reports the start of an individual test case.
     *
     * @param test identifies the test
     */
    public default void testStarted(TestIdentifier test) {}

    /**
     * Alternative to {@link #testStarted(TestIdentifier)} where we also specify when the test was
     * started, combined with {@link #testEnded(TestIdentifier, long, Map)} for accurate measure.
     *
     * @param test identifies the test
     * @param startTime the time the test started, measured via {@link System#currentTimeMillis()}
     */
    default void testStarted(TestIdentifier test, long startTime) {
        testStarted(test);
    }

    /**
     * Reports the failure of a individual test case.
     *
     * <p>Will be called between testStarted and testEnded.
     *
     * @param test identifies the test
     * @param trace stack trace of failure
     */
    public default void testFailed(TestIdentifier test, String trace) {}

    /**
     * Called when an atomic test flags that it assumes a condition that is false
     *
     * @param test identifies the test
     * @param trace stack trace of failure
     */
    public default void testAssumptionFailure(TestIdentifier test, String trace) {}

    /**
     * Called when a test will not be run, generally because a test method is annotated with
     * org.junit.Ignore.
     *
     * @param test identifies the test
     */
    public default void testIgnored(TestIdentifier test) {}

    /**
     * Reports the execution end of an individual test case.
     *
     * <p>If {@link #testFailed} was not invoked, this test passed. Also returns any key/value
     * metrics which may have been emitted during the test case's execution.
     *
     * @param test identifies the test
     * @param testMetrics a {@link Map} of the metrics emitted
     */
    public default void testEnded(TestIdentifier test, Map<String, String> testMetrics) {}

    /**
     * Alternative to {@link #testEnded(TestIdentifier, Map)} where we can specify the end time
     * directly. Combine with {@link #testStarted(TestIdentifier, long)} for accurate measure.
     *
     * @param test identifies the test
     * @param endTime the time the test ended, measured via {@link System#currentTimeMillis()}
     * @param testMetrics a {@link Map} of the metrics emitted
     */
    public default void testEnded(
            TestIdentifier test, long endTime, Map<String, String> testMetrics) {
        testEnded(test, testMetrics);
    }
}
