#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "Assert.h"


#ifdef __cplusplus
extern "C" {
#endif


#ifndef TEST
#error Tests must be compiled with the TEST macro defined.
#endif


#define TEST_CASE(X) \
	void TEST_CASE_ ## X(); \
	TestCaseInfo _TEST_ ## X ## _INFO = { .next = 0, .test = TEST_CASE_ ## X, .name = #X, .file = __FILE__, .line = __LINE__ }; \
	static __attribute__((constructor)) void _TEST_ ## X ## _INIT() { AddTestCaseToList(&_TEST_ ## X ## _INFO); } \
	void TEST_CASE_ ## X()


#define TEST_SETUP(X) TEST_HELPER(X, SETUP)
#define TEST_START(X) TEST_HELPER(X, START)
#define TEST_FINISH(X) TEST_HELPER(X, FINISH)
#define TEST_TEARDOWN(X) TEST_HELPER(X, TEARDOWN)

#define TEST_HELPER(NAME, TYPE) \
	void TEST_HELPER_ ## TYPE ## _ ## NAME(); \
	TestHelperInfo _TEST_HELPER_ ## TYPE ## _ ## NAME ## _INFO = { .next = 0, .fn = TEST_HELPER_ ## TYPE ## _ ## NAME, .type = TEST_HELPER_ ## TYPE }; \
	static __attribute__((constructor)) void _TEST_HELPER_ ## TYPE ## _INIT() { AddTestHelperToList(&_TEST_HELPER_ ## TYPE ## _ ## NAME ## _INFO); } \
	void TEST_HELPER_ ## TYPE ## _ ## NAME()


typedef void(*TestCaseFn)();
typedef void(*TestHelperFn)();


typedef enum TestHelperType
{
	TEST_HELPER_SETUP,
	TEST_HELPER_START,
	TEST_HELPER_FINISH,
	TEST_HELPER_TEARDOWN,
} TestHelperType;


typedef struct TestCaseInfo
{
	struct TestCaseInfo* next;
	TestCaseFn	test;
	const char*	name;
	const char* file;
	int			line;
} TestCaseInfo;


typedef struct TestHelperInfo
{
	struct TestHelperInfo* next;
	TestHelperFn	fn;
	TestHelperType	type;
} TestHelperInfo;


extern void AddTestCaseToList(TestCaseInfo* info);
extern void AddTestHelperToList(TestHelperInfo* info);

extern bool RunTestCase(const TestCaseInfo* info);


#ifdef __cplusplus
}
#endif

