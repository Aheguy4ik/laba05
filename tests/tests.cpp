#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "Account.h"
#include "Transaction.h"

using ::testing::Return;
using ::testing::Throw;
using ::testing::_;
using ::testing::HasSubstr;
using ::testing::NiceMock;
using ::testing::StrictMock;

// --- Мок для Account ---
class MockAccount : public Account {
public:
    MockAccount() : Account(0, 0) {}

    MOCK_CONST_METHOD0(id, int());
    MOCK_CONST_METHOD0(GetBalance, int());
    MOCK_METHOD1(ChangeBalance, void(int diff));
    MOCK_METHOD0(Lock, void());
    MOCK_METHOD0(Unlock, void());
};

// --- Тесты Transaction через MockAccount ---

TEST(TransactionMock, CallsAccountMethodsOnSuccess) {
    StrictMock<MockAccount> from;
    StrictMock<MockAccount> to;

    Transaction tr;
    tr.set_fee(50);

    EXPECT_CALL(from, id()).WillRepeatedly(Return(0));
    EXPECT_CALL(to, id()).WillRepeatedly(Return(1));

    EXPECT_CALL(from, GetBalance()).WillOnce(Return(1000)).WillOnce(Return(850));
    EXPECT_CALL(to, GetBalance()).WillOnce(Return(200));
    EXPECT_CALL(from, Lock()).Times(1);
    EXPECT_CALL(to, Lock()).Times(1);
    EXPECT_CALL(from, Unlock()).Times(1);
    EXPECT_CALL(to, Unlock()).Times(1);

    EXPECT_CALL(from, ChangeBalance(-150)).Times(1);
    EXPECT_CALL(to, ChangeBalance(100)).Times(1);

    testing::internal::CaptureStdout();
    EXPECT_TRUE(tr.Make(from, to, 100));
    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_THAT(output, HasSubstr("0 send to 1 $100"));
}

TEST(TransactionMock, FailsWhenInsufficientFunds) {
    StrictMock<MockAccount> from;
    StrictMock<MockAccount> to;

    Transaction tr;
    tr.set_fee(60);

    EXPECT_CALL(from, id()).WillRepeatedly(Return(0));
    EXPECT_CALL(to, id()).WillRepeatedly(Return(1));
    EXPECT_CALL(from, GetBalance()).WillOnce(Return(100));
    EXPECT_CALL(from, Lock());
    EXPECT_CALL(from, Unlock());

    EXPECT_CALL(to, Lock());
    EXPECT_CALL(to, Unlock());

    EXPECT_FALSE(tr.Make(from, to, 100));
}

TEST(TransactionMock, ThrowsIfSameAccount) {
    Transaction tr;
    StrictMock<MockAccount> acc;

    EXPECT_CALL(acc, id()).WillRepeatedly(Return(1));

    EXPECT_THROW(tr.Make(acc, acc, 100), std::logic_error);
}

TEST(TransactionMock, ThrowsIfNegativeAmount) {
    Transaction tr;
    StrictMock<MockAccount> from, to;

    EXPECT_CALL(from, id()).WillOnce(Return(0));
    EXPECT_CALL(to, id()).WillOnce(Return(1));

    EXPECT_THROW(tr.Make(from, to, -50), std::invalid_argument);
}

TEST(TransactionMock, ThrowsIfTooSmallAmount) {
    Transaction tr;
    StrictMock<MockAccount> from, to;

    EXPECT_CALL(from, id()).WillOnce(Return(0));
    EXPECT_CALL(to, id()).WillOnce(Return(1));

    EXPECT_THROW(tr.Make(from, to, 50), std::logic_error); // < 100
}

TEST(TransactionMock, ReturnsFalseWhenFeeTooHigh) {
    Transaction tr;
    tr.set_fee(60); // 60 * 2 = 120 > 100

    StrictMock<MockAccount> from, to;

    EXPECT_CALL(from, id()).WillRepeatedly(Return(0));
    EXPECT_CALL(to, id()).WillRepeatedly(Return(1));

    EXPECT_FALSE(tr.Make(from, to, 100));
}
