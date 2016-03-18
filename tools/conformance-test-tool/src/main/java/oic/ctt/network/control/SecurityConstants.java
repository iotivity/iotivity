package oic.ctt.network.control;

/**
 * Commonly used Security Constants are defined as enum in this class
 *
 * @author Tanvir Ferdousi (tanvir.f@samsung.com)
 *
 */
public class SecurityConstants {

    public enum ArgumentType {
        IP_V4_ADDRESS(1), IP_V6_ADDRESS(2), PORT(3), CIPHER_SUITE(
                4), CLIENT_IDENTITY(5), SERVER_IDENTITY(6), PASSWORD(
                        7), PRIVATE_DATA(8);

        private int mArgumentTypeValue;

        private ArgumentType(int argumentTypeValue) {
            this.mArgumentTypeValue = argumentTypeValue;
        }

        public static ArgumentType parseArgumentType(int argumentTypeValue) {
            switch (argumentTypeValue) {
                case 1:
                    return IP_V4_ADDRESS;
                case 2:
                    return IP_V6_ADDRESS;
                case 3:
                    return PORT;
                case 4:
                    return CIPHER_SUITE;
                case 5:
                    return CLIENT_IDENTITY;
                case 6:
                    return SERVER_IDENTITY;
                case 7:
                    return PASSWORD;
                case 8:
                    return PRIVATE_DATA;
                default:
                    throw new IllegalArgumentException("Invalid Argument Code");
            }
        }

        public int getValue() {
            return this.mArgumentTypeValue;
        }
    }

    public enum MessageCode {
        INIT_DTLS_CONNECTION(1), TERMINATE_DTLS_CONNECTION(
                2), DTLS_HANDSHAKE_SUCCESS(101), DTLS_HANDSHAKE_FAILURE(
                        201), DTLS_TERMINATE_SUCCESS(
                                102), DTLS_TERMINATE_FAILURE(202);

        private int mMessageCodeValue;

        private MessageCode(int messageCodeValue) {
            this.mMessageCodeValue = messageCodeValue;
        }

        public static MessageCode parseMessageCode(int messageCodeValue) {
            switch (messageCodeValue) {
                case 1:
                    return INIT_DTLS_CONNECTION;
                case 2:
                    return TERMINATE_DTLS_CONNECTION;
                case 101:
                    return DTLS_HANDSHAKE_SUCCESS;
                case 201:
                    return DTLS_HANDSHAKE_FAILURE;
                case 102:
                    return DTLS_TERMINATE_SUCCESS;
                case 202:
                    return DTLS_TERMINATE_FAILURE;
                default:
                    throw new IllegalArgumentException("Invalid Message Code");
            }
        }

        public int getValue() {
            return this.mMessageCodeValue;
        }
    }

    public enum CipherSuite {
        TLS_NULL_WITH_NULL_NULL(0x0000), TLS_ECDH_anon_WITH_AES_128_CBC_SHA_256(
                0xC018), TLS_PSK_WITH_AES_128_CCM_8(
                        0xC0A8), TLS_ECDHE_PSK_WITH_AES_128_CBC_SHA_256(
                                0xC037), TLS_ECDHE_ECDSA_WITH_AES_128_CCM_8(
                                        0xC0AE);

        private int mCipherSuiteValue;

        private CipherSuite(int cipherSuiteValue) {
            this.mCipherSuiteValue = cipherSuiteValue;
        }

        public static CipherSuite parseCipherSuite(int cipherSuiteValue) {
            switch (cipherSuiteValue) {
                case 0x0000:
                    return TLS_NULL_WITH_NULL_NULL;
                case 0xC018:
                    return TLS_ECDH_anon_WITH_AES_128_CBC_SHA_256;
                case 0xC0A8:
                    return TLS_PSK_WITH_AES_128_CCM_8;
                case 0xC037:
                    return TLS_ECDHE_PSK_WITH_AES_128_CBC_SHA_256;
                case 0xC0AE:
                    return TLS_ECDHE_ECDSA_WITH_AES_128_CCM_8;
                default:
                    throw new IllegalArgumentException("Invalid Cipher Suite");
            }
        }

        public int getValue() {
            return this.mCipherSuiteValue;
        }
    }

}
