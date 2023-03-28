#include "pch.h"
#include "CppUnitTest.h"
#include <vector>
#include <algorithm>
#include <iostream>
#include <string>
#include "..\circular buffer\circular_buffer.h"
#include "..\circular buffer\dynamic_circular_buffer.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace buffertests
{
	TEST_CLASS(static_buffer)
	{
	public:
		TEST_METHOD(test_default_constructor)
		{
			circular_buffer <int, 5> a;
			std::vector <int> b(5);
			Assert::IsTrue(std::equal(a.begin(), a.end(), b.begin()));
		}
		TEST_METHOD(test_initializer_list)
		{
			circular_buffer <double, 5> a = {1.2,2.3,3.1,4.4,5.562};
			std::vector <double> b = { 1.2,2.3,3.1,4.4,5.562 };
			Assert::IsTrue(std::equal(a.begin(), a.end(), b.begin()));
		}
		TEST_METHOD(test_n_val_initialization)
		{
			circular_buffer <std::string, 2> a("abc");
			std::vector <std::string> b(2, "abc");
			Assert::IsTrue(std::equal(a.begin(), a.end(), b.begin()));
		}
		TEST_METHOD(test_iterators_initialization)
		{
			std::vector<std::vector<int>> origin = { {1,2,3}, {4,5,6}, {7,8,9} };
			std::vector <std::vector<int>> b(origin.begin(), origin.end());
			circular_buffer <std::vector<int>, 3> a(origin.begin(), origin.end());
			Assert::IsTrue(std::equal(a.begin(), a.end(), b.begin()));
		}
		TEST_METHOD(test_copy)
		{
			std::vector<int> b = { 1,2,3 };
			circular_buffer <int, 3> origin = { 1,2,3 };
			circular_buffer <int, 3> a = origin;
			Assert::IsTrue(std::equal(a.begin(), a.end(), b.begin()));
		}
		TEST_METHOD(test_r_copy)
		{
			std::vector<int> b = { 1,2,3 };
			circular_buffer <int, 3> origin = { 1,2,3 };
			circular_buffer <int, 3> a = std::move(origin);
			Assert::IsTrue(std::equal(a.begin(), a.end(), b.begin()));
		}
		TEST_METHOD(test_equal)
		{
			std::vector<int> b = { 1,2,3 };
			circular_buffer <int, 3> origin = { 1,2,3 };
			circular_buffer <int, 3> a;
			a = origin;
			Assert::IsTrue(std::equal(a.begin(), a.end(), b.begin()));
		}
		TEST_METHOD(test_r_equal)
		{
			std::vector<int> b = { 1,2,3 };
			circular_buffer <int, 3> origin = { 1,2,3 };
			circular_buffer <int, 3> a;
			a = std::move(origin);
			Assert::IsTrue(std::equal(a.begin(), a.end(), b.begin()));
		}
		TEST_METHOD(test_begin)
		{
			circular_buffer <int, 3> a = {1,2,3};
			Assert::IsTrue(*(a.begin()) == 1);
		}
		TEST_METHOD(test_end)
		{
			circular_buffer <int, 3> a = { 1,2,3 };
			Assert::IsTrue(*(a.end() - 1) == 3);
		}
		TEST_METHOD(test_cbegin)
		{
			circular_buffer <int, 3> a = { 1,2,3 };
			Assert::IsTrue(*(a.cbegin()) == 1);
		}
		TEST_METHOD(test_cend)
		{
			circular_buffer <int, 3> a = { 1,2,3 };
			Assert::IsTrue(*(a.cend() - 1) == 3);
		}
		TEST_METHOD(test_brackets)
		{
			circular_buffer <int, 3> a = { 1,2,3 };
			Assert::IsTrue(a[1] == 2);
		}
		TEST_METHOD(test_at)
		{
			circular_buffer <int, 3> a = { 1,2,3 };
			Assert::IsTrue(a.at(1) == 2);
		}
		TEST_METHOD(test_size)
		{
			circular_buffer <int, 3> a = { 1,2,3 };
			Assert::IsTrue(a.size() == 3);
		}
		TEST_METHOD(test_front)
		{
			circular_buffer <int, 3> a = { 1,2,3 };
			Assert::IsTrue(a.front() == 1);
		}
		TEST_METHOD(test_back)
		{
			circular_buffer <int, 3> a = { 1,2,3 };
			Assert::IsTrue(a.back() == 3);
		}
		TEST_METHOD(test_clear)
		{
			circular_buffer <int, 3> a = { 1,2,3 };
			std::vector<int> b(3);
			a.clear();
			Assert::IsTrue(std::equal(a.begin(), a.end(), b.begin()));
		}
		TEST_METHOD(test_swap)
		{
			circular_buffer <int, 3> a = { 1,2,3 };
			circular_buffer <int, 3> origin = { 0,1,4 };
			std::vector<int> b = {0,1,4};
			a.swap(origin);
			Assert::IsTrue(std::equal(a.begin(), a.end(), b.begin()));
		}
		TEST_METHOD(test_insert_two_iters)
		{
			circular_buffer <int, 3> a = { 1,2,3 };
			std::vector<int> origin = { 1,-5,3 };
			a.insert(a.begin() + 1, origin.begin() + 1, origin.begin() + 2);
			std::vector<int> b = { 1,-5,3 };
			Assert::IsTrue(std::equal(a.begin(), a.end(), b.begin()));
		}
		TEST_METHOD(test_insert_iter)
		{
			circular_buffer <int, 3> a = { 1,2,3 };
			a.insert(a.begin() + 1, -5);
			std::vector<int> b = { 1,-5,3 };
			Assert::IsTrue(std::equal(a.begin(), a.end(), b.begin()));
		}
		TEST_METHOD(test_insert_n_val)
		{
			circular_buffer <int, 3> a = { 1,2,1 };
			a.insert(a.begin(), static_cast<size_t>(2), 4);
			std::vector<int> b = { 4,4,1 };
			Assert::IsTrue(std::equal(a.begin(), a.end(), b.begin()));
		}
		TEST_METHOD(test_push_back)
		{
			circular_buffer <int, 3> a = { 1,2,1 };
			a.push_back(4);
			a.push_back(std::move(4));
			std::vector<int> b = { 4,4,1 };
			Assert::IsTrue(std::equal(a.begin(), a.end(), b.begin()));
		}
	};
	TEST_CLASS(dynamic_buffer)
	{
	public:
		TEST_METHOD(test_default_constructor)
		{
			dynamic_circular_buffer <int> a;
			std::vector <int> b(5);
			Assert::IsTrue(std::equal(a.begin(), a.end(), b.begin()));
		}
		TEST_METHOD(test_initializer_list)
		{
			dynamic_circular_buffer <double> a = { 1.2,2.3,3.1,4.4,5.562 };
			std::vector <double> b = { 1.2,2.3,3.1,4.4,5.562 };
			Assert::IsTrue(std::equal(a.begin(), a.end(), b.begin()));
		}
		TEST_METHOD(test_n_val_initialization)
		{
			dynamic_circular_buffer <std::string> a(2, "abc");
			std::vector <std::string> b(2, "abc");
			Assert::IsTrue(std::equal(a.begin(), a.end(), b.begin()));
		}
		TEST_METHOD(test_iterators_initialization)
		{
			std::vector<std::vector<int>> origin = { {1,2,3}, {4,5,6}, {7,8,9} };
			std::vector <std::vector<int>> b(origin.begin(), origin.end());
			dynamic_circular_buffer<std::vector<int>> a(origin.begin(), origin.end());
			Assert::IsTrue(std::equal(a.begin(), a.end(), b.begin()));
		}
		TEST_METHOD(test_copy)
		{
			std::vector<int> b = { 1,2,3 };
			dynamic_circular_buffer <int> origin = { 1,2,3 };
			dynamic_circular_buffer <int> a = origin;
			Assert::IsTrue(std::equal(a.begin(), a.end(), b.begin()));
		}
		TEST_METHOD(test_r_copy)
		{
			std::vector<int> b = { 1,2,3 };
			dynamic_circular_buffer <int> origin = { 1,2,3 };
			dynamic_circular_buffer <int> a = std::move(origin);
			Assert::IsTrue(std::equal(a.begin(), a.end(), b.begin()));
		}
		TEST_METHOD(test_equal)
		{
			std::vector<int> b = { 1,2,3 };
			dynamic_circular_buffer <int> origin = { 1,2,3 };
			dynamic_circular_buffer <int> a;
			a = origin;
			Assert::IsTrue(std::equal(a.begin(), a.end(), b.begin()));
		}
		TEST_METHOD(test_r_equal)
		{
			std::vector<int> b = { 1,2,3 };
			dynamic_circular_buffer <int> origin = { 1,2,3 };
			dynamic_circular_buffer <int> a;
			a = std::move(origin);
			Assert::IsTrue(std::equal(a.begin(), a.end(), b.begin()));
		}
		TEST_METHOD(test_begin)
		{
			dynamic_circular_buffer <int> a = { 1,2,3 };
			Assert::IsTrue(*(a.begin()) == 1);
		}
		TEST_METHOD(test_end)
		{
			dynamic_circular_buffer <int> a = { 1,2,3 };
			Assert::IsTrue(*(a.end() - 1) == 3);
		}
		TEST_METHOD(test_cbegin)
		{
			dynamic_circular_buffer <int> a = { 1,2,3 };
			Assert::IsTrue(*(a.cbegin()) == 1);
		}
		TEST_METHOD(test_cend)
		{
			dynamic_circular_buffer <int> a = { 1,2,3 };
			Assert::IsTrue(*(a.cend() - 1) == 3);
		}
		TEST_METHOD(test_brackets)
		{
			dynamic_circular_buffer <int> a = { 1,2,3 };
			Assert::IsTrue(a[1] == 2);
		}
		TEST_METHOD(test_at)
		{
			dynamic_circular_buffer <int> a = { 1,2,3 };
			Assert::IsTrue(a.at(1) == 2);
		}
		TEST_METHOD(test_size)
		{
			dynamic_circular_buffer <int> a = { 1,2,3 };
			Assert::IsTrue(a.size() == 3);
		}
		TEST_METHOD(test_front)
		{
			dynamic_circular_buffer <int> a = { 1,2,3 };
			Assert::IsTrue(a.front() == 1);
		}
		TEST_METHOD(test_back)
		{
			dynamic_circular_buffer <int> a = { 1,2,3 };
			Assert::IsTrue(a.back() == 3);
		}
		TEST_METHOD(test_empty)
		{
			dynamic_circular_buffer <int> a = { 1,2,3 };
			dynamic_circular_buffer <int> b = { 1,2,3 };
			b.clear();
			Assert::IsTrue((a.empty() == false) && (b.empty() == true));
		}
		TEST_METHOD(test_clear)
		{
			dynamic_circular_buffer <int> a = { 1,2,3 };
			std::vector<int> b(3);
			a.clear();
			Assert::IsTrue(std::equal(a.begin(), a.end(), b.begin()));
		}
		TEST_METHOD(test_swap)
		{
			dynamic_circular_buffer <int> a = { 1,2,3 };
			dynamic_circular_buffer <int> origin = { 0,1,4 };
			std::vector<int> b = { 0,1,4 };
			a.swap(origin);
			Assert::IsTrue(std::equal(a.begin(), a.end(), b.begin()));
		}
		TEST_METHOD(test_insert_two_iters)
		{
			dynamic_circular_buffer <int> a = { 1,2,3 };
			std::vector<int> origin = { 1,-5,3 };
			a.insert(a.begin() + 1, origin.begin() + 1, origin.begin() + 2);
			std::vector<int> b = { 1,-5,3 };
			Assert::IsTrue(std::equal(a.begin(), a.end(), b.begin()));
		}
		TEST_METHOD(test_insert_iter)
		{
			dynamic_circular_buffer <int> a = { 1,2,3 };
			a.insert(a.begin() + 1, -5);
			std::vector<int> b = { 1,-5,3 };
			Assert::IsTrue(std::equal(a.begin(), a.end(), b.begin()));
		}
		TEST_METHOD(test_insert_n_val)
		{
			dynamic_circular_buffer <int> a = { 1,2,1 };
			a.insert(a.begin(), static_cast<size_t>(2), 4);
			std::vector<int> b = { 4,4,1 };
			Assert::IsTrue(std::equal(a.begin(), a.end(), b.begin()));
		}
		TEST_METHOD(test_push_back)
		{
			dynamic_circular_buffer <int> a = { 1,2,1 };
			a.push_back(4);
			a.push_back(std::move(4));
			std::vector<int> b = { 4,4,1 };
			Assert::IsTrue(std::equal(a.begin(), a.end(), b.begin()));
		}
		TEST_METHOD(test_erase)
		{
			dynamic_circular_buffer <int> a = { 1,2,1 };
			a.erase(a.begin() + 1);
			std::vector<int> b = { 1,1 };
			Assert::IsTrue(std::equal(a.begin(), a.end(), b.begin()));
		}
		TEST_METHOD(test_pop_back)
		{
			dynamic_circular_buffer <int> a = { 1,2,1 };
			a.pop_back();
			std::vector<int> b = { 1,2 };
			Assert::IsTrue(std::equal(a.begin(), a.end(), b.begin()));
		}
		TEST_METHOD(test_pop_front)
		{
			dynamic_circular_buffer <int> a = { 1,2,1 };
			a.pop_front();
			std::vector<int> b = { 2,1 };
			Assert::IsTrue(std::equal(a.begin(), a.end(), b.begin()));
		}
	};
}
