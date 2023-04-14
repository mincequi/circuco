#include <unity.h>

#include <ConfigRepository.hpp>

class FileDataSourceMock : public FileDataSourceInterface {
    virtual std::string htmlTemplate() const override {};
    virtual std::string css() const override {};
    virtual std::string config() const override {
        return "aps = ["
        "{ ssid = \"Ssid1\", password = \"Password1\" },"
        "{ ssid = \"Ssid2\", password = \"Password2\" }"
        "]";
    }
};

FileDataSourceMock fileDataSourceMock;
ConfigRepository cut(fileDataSourceMock);

void setUp(void) {
  // set stuff up here
}

void tearDown(void) {
  // clean stuff up here
}

void ConfigRepository_parses_aps(void) {
    TEST_ASSERT(cut.aps().size() == 2);
    TEST_ASSERT(cut.aps().at(0).first == "Ssid1");
    TEST_ASSERT(cut.aps().at(1).second == "Password2");
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(ConfigRepository_parses_aps);
    return UNITY_END();
}
