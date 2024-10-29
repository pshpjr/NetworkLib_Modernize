#include <iostream>

#include "gtest/gtest.h"

namespace
{
	class Test1 : public ::testing::Test
	{
	public:

	protected:
		virtual void SetUp()
		{
			std::cout << "Setup" << std::endl;
		}

		virtual void TearDown()
		{
			std::cout << "TearDown" << std::endl;
		}
	};

	TEST_F(Test1, plus_p)
	{
		EXPECT_EQ(2, 1+1);
	}
} // namespace

int main(int argc, char** argv)
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
