#include "unity.h"
#include "unity_fixture.h"

TEST_GROUP_RUNNER(User_IODetectErr)
{
	RUN_TEST_CASE(User_IODetectErr, alreadHaveAnErrNotDetectErr);
	RUN_TEST_CASE(User_IODetectErr, noErrThenDetect1stErrAtAddress0);
	RUN_TEST_CASE(User_IODetectErr, noErrThenDetect2stErrAtAddress0);
	RUN_TEST_CASE(User_IODetectErr, noErrThenDetect3stErrAtAddress0);
	RUN_TEST_CASE(User_IODetectErr, noErrThenDetect4stErrAtAddress0);
	RUN_TEST_CASE(User_IODetectErr, noErrThenDetect1stErrAnd1stNoErrAtAddress0)
}