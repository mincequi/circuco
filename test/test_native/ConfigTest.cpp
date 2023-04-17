#include <unity.h>

#include <bl/Config.hpp>
#include <io/TimeBase.hpp>

class FileSystemMock : public FileSystemBase {
    virtual std::string htmlTemplate() const override {};
    virtual std::string css() const override {};
    virtual std::string config() const override {
        return "aps = ["
        "{ ssid = \"Ssid1\", password = \"Password1\" },"
        "{ ssid = \"Ssid2\", password = \"Password2\" }"
        "]";
    }
};

class TimeMock : public TimeBase {
    virtual void doLoop() override {
    }
};

FileSystemMock fileSystemMock;
TimeMock timeMock;

Config cut(fileSystemMock, timeMock);

void setUp(void) {
    cut.setup();
}

void tearDown(void) {
}

void config_parses_aps(void) {
    TEST_ASSERT_EQUAL_INT(2, cut.aps().size());
    TEST_ASSERT(cut.aps().at(0).first == "Ssid1");
    TEST_ASSERT(cut.aps().at(1).second == "Password2");
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(config_parses_aps);
    return UNITY_END();
}
