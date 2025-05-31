#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "Account.h"
#include "Transaction.h"

using ::testing::Return;
using ::testing::Throw;
using ::testing::_;

// --- Мок для Account ---
class MockAccount : public Account {
public:
    MockAccount(int id, int balance) : Account(id, balance) {}

    MOCK_CONST_METHOD0(GetBalance, int());
    MOCK_METHOD1(ChangeBalance, void(int diff));
    MOCK_METHOD0(Lock, void());
    MOCK_METHOD0(Unlock, void());
};

// --- Тесты Transaction через MockAccount ---

TEST(TransactionMock, CallsAccountMethodsOnSuccess) {
    MockAccount from(0, 1000);
    MockAccount to(1, 100);

    Transaction tr;
    tr.set_fee(50);

    // Подготовка вызовов
    EXPECT_CALL(from, GetBalance()).WillRepeatedly(Return(1000));
    EXPECT_CALL(from, Lock()).Times(1);
    EXPECT_CALL(from, Unlock()).Times(1);
    EXPECT_CALL(from, ChangeBalance(-150)).Times(1);

    EXPECT_CALL(to, Lock()).Times(1);
    EXPECT_CALL(to, Unlock()).Times(1);
    EXPECT_CALL(to, ChangeBalance(100)).Times(1);

    EXPECT_TRUE(tr.Make(from, to, 100));
}

TEST(TransactionMock, FailsWhenInsufficientFunds) {
    MockAccount from(0, 100);
    MockAccount to(1, 100);

    Transaction tr;
    tr.set_fee(50);

    EXPECT_CALL(from, GetBalance()).WillOnce(Return(100));
    EXPECT_CALL(from, Lock()).Times(1);
    EXPECT_CALL(from, Unlock()).Times(1);

    EXPECT_FALSE(tr.Make(from, to, 100));
}
