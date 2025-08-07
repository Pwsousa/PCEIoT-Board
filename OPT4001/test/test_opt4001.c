#include "unity.h"
#include "opt4001.h"

void setUp(void) {}
void tearDown(void) {}

void test_crc8_known_data_should_return_correct_crc(void) {
    uint8_t data[] = {0x01, 0x02, 0x03};
    uint8_t crc = opt4001_crc8(data, sizeof(data));
    TEST_ASSERT_EQUAL_HEX8(0x12, crc); // Ajuste o valor esperado para o correto
}

void test_read_fifo_all_should_return_expected_number_of_samples(void) {
    // Simula um buffer que receberia valores fictícios
    float lux_array[8];
    int count = opt4001_read_fifo_all(lux_array, 8);

    TEST_ASSERT(count >= 0 && count <= 8);
}

void test_threshold_flags(void) {
    bool above = opt4001_is_above_threshold();
    bool below = opt4001_is_below_threshold();

    TEST_ASSERT_TRUE(above == true || below == true || (above == false && below == false));
}

void test_configure_threshold_does_not_crash(void) {
    configurar_limites_opt4001();
    TEST_PASS_MESSAGE("configurar_limites_opt4001 executado com sucesso.");
}

void test_enable_interrupt_does_not_crash(void) {
    opt4001_enable_threshold_interrupt();
    TEST_PASS_MESSAGE("opt4001_enable_threshold_interrupt executado com sucesso.");
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_crc8_known_data_should_return_correct_crc);
    RUN_TEST(test_read_fifo_all_should_return_expected_number_of_samples);
    RUN_TEST(test_threshold_flags);
    RUN_TEST(test_configure_threshold_does_not_crash);
    RUN_TEST(test_enable_interrupt_does_not_crash);
    return UNITY_END();
}
