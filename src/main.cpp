/**************************************************************************
 * Copyright 2018 Advanced Micro Devices, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 **************************************************************************/

#include "commands.h"  // has measurement_t
#include "tests.h"     // for test_all
#include "utilities.h" // for str_to_array
#include <getopt.h>    // for getopt_long
#include <stdio.h>
#include <string>

char help_array[] =  "The following commands are supported:\n" \
                    " sevtool -[global opts] --[command] [command opts]\n" \
                    "(Please see the readme file for more detailed information)\n" \
                    "Platform Owner commands:\n" \
                    "  factory_reset\n" \
                    "  platform_status\n" \
                    "  pek_gen\n" \
                    "  pek_csr\n" \
                    "  pdh_gen\n" \
                    "  pdh_cert_export\n" \
                    "  pek_cert_import\n" \
                    "      Input params:\n" \
                    "          [oca private key].pem file\n" \
                    "  get_id\n" \
                    "  set_self_owed\n" \
                    "  set_externally_owned\n" \
                    "      Input params:\n" \
                    "          [oca private key].pem file\n" \
                    "  generate_cek_ask\n" \
                    "  get_ask_ark\n" \
                    "  export_cert_chain\n" \
                    "Guest Owner commands:\n" \
                    "  calc_measurement\n" \
                    "      Input params (all in ascii-encoded hex bytes):\n" \
                    "          uint8_t  meas_ctx\n" \
                    "          uint8_t  api_major\n" \
                    "          uint8_t  api_minor\n" \
                    "          uint8_t  build_id\n" \
                    "          uint32_t policy\n" \
                    "          uint32_t digest\n" \
                    "          uint8_t  m_nonce[128/8]\n" \
                    "          uint8_t  gctx_tik[128/8]\n" \
                    "  validate_cert_chain\n" \
                    "  generate_launch_blob\n" \
                    "      Input params:\n" \
                    "          uint32_t policy\n" \
                    "  package_secret\n" \
                    "      Input params:\n" \
                    "          launch_blob.txt file\n" \
                    ;

/* Flag set by '--verbose' */
static int verbose_flag = 0;

static struct option long_options[] =
{
    /* These options set a flag. */
    {"verbose",             no_argument,       &verbose_flag, 1},
    {"brief",               no_argument,       &verbose_flag, 0},

    /* These options don't set a flag. We distinguish them by their indices. */
    /* Platform Owner commands */
    {"factory_reset",        no_argument,       0, 'a'},
    {"platform_status",      no_argument,       0, 'b'},
    {"pek_gen",              no_argument,       0, 'c'},
    {"pek_csr",              no_argument,       0, 'd'},
    {"pdh_gen",              no_argument,       0, 'e'},
    {"pdh_cert_export",      no_argument,       0, 'f'},
    {"pek_cert_import",      required_argument, 0, 'g'},
    {"get_id",               no_argument,       0, 'j'},
    {"set_self_owned",       no_argument,       0, 'k'},
    {"set_externally_owned", required_argument, 0, 'l'},
    {"generate_cek_ask",     no_argument,       0, 'm'},
    {"get_ask_ark",          no_argument,       0, 'n'},
    {"export_cert_chain",    no_argument,       0, 'p'},
    /* Guest Owner commands */
    {"calc_measurement",     required_argument, 0, 't'},
    {"validate_cert_chain",  no_argument,       0, 'u'},
    {"generate_launch_blob", required_argument, 0, 'v'},
    {"package_secret",       no_argument,       0, 'w'},

    /* Run tests */
    {"test_all",             no_argument,       0, 'T'},

    {"help",                 no_argument,       0, 'H'},
    {"sys_info",             no_argument,       0, 'I'},
    {"ofolder",              required_argument, 0, 'O'},
    {0, 0, 0, 0}
};

int main(int argc, char** argv)
{
    int c = 0;
    int option_index = 0;   /* getopt_long stores the option index here. */
    std::string output_folder = "./";

    int cmd_ret = 0xFFFF;

    while ((c = getopt_long (argc, argv, "hio:", long_options, &option_index)) != -1)
    {
        switch (c) {
            case 'h':           // help
            case 'H': {
                printf("%s\n", help_array);
                cmd_ret = 0;
                break;
            }
            case 'i':           // sys_info
            case 'I': {
                Command cmd(output_folder, verbose_flag);
                cmd_ret = cmd.sys_info();  // Display system info
                break;
            }
            case 'o':           // ofolder
            case 'O': {
                output_folder = optarg;
                output_folder += "/";
                break;
            }
            case 'a': {         // PLATFORM_RESET
                Command cmd(output_folder, verbose_flag);
                cmd_ret = cmd.factory_reset();
                break;
            }
            case 'b': {         // PLATFORM_STATUS
                Command cmd(output_folder, verbose_flag);
                cmd_ret = cmd.platform_status();
                break;
            }
            case 'c': {         // PEK_GEN
                Command cmd(output_folder, verbose_flag);
                cmd_ret = cmd.pek_gen();
                break;
            }
            case 'd': {         // PEK_CSR
                Command cmd(output_folder, verbose_flag);
                cmd_ret = cmd.pek_csr();
                break;
            }
            case 'e': {         // PDH_GEN
                Command cmd(output_folder, verbose_flag);
                cmd_ret = cmd.pdh_gen();
                break;
            }
            case 'f': {         // PDH_CERT_EXPORT
                Command cmd(output_folder, verbose_flag);
                cmd_ret = cmd.pdh_cert_export();
                break;
            }
            case 'g': {         // PEK_CERT_IMPORT
                optind--;   // Can't use option_index because it doesn't account for '-' flags
                if(argc - optind != 1) {
                    printf("Error: Expecting exactly 1 arg\n");
                    break;
                }

                std::string oca_priv_key_file = argv[optind++];
                Command cmd(output_folder, verbose_flag);
                cmd_ret = cmd.pek_cert_import(oca_priv_key_file);
                break;
            }
            case 'j': {         // GET_ID
                Command cmd(output_folder, verbose_flag);
                cmd_ret = cmd.get_id();
                break;
            }
            case 'k': {         // SET_SELF_OWNED
                Command cmd(output_folder, verbose_flag);
                cmd_ret = cmd.set_self_owned();
                break;
            }
            case 'l': {         // SET_EXTERNALLY_OWNED
                optind--;   // Can't use option_index because it doesn't account for '-' flags
                if(argc - optind != 1) {
                    printf("Error: Expecting exactly 1 args\n");
                    break;
                }

                std::string oca_priv_key_file = argv[optind++];
                Command cmd(output_folder, verbose_flag);
                cmd_ret = cmd.set_externally_owned(oca_priv_key_file);
                break;
            }
            case 'm': {         // GENERATE_CEK_ASK
                Command cmd(output_folder, verbose_flag);
                cmd_ret = cmd.generate_cek_ask();
                break;
            }
            case 'n': {         // GET_ASK_ARK
                Command cmd(output_folder, verbose_flag);
                cmd_ret = cmd.get_ask_ark();
                break;
            }
            case 'p': {         // EXPORT_CERT_CHAIN
                Command cmd(output_folder, verbose_flag);
                cmd_ret = cmd.export_cert_chain();
                break;
            }
            case 't': {         // CALC_MEASUREMENT
                optind--;   // Can't use option_index because it doesn't account for '-' flags
                if(argc - optind != 8) {
                    printf("Error: Expecting exactly 8 args\n");
                    break;
                }

                measurement_t user_data;
                user_data.meas_ctx  = (uint8_t)strtol(argv[optind++], NULL, 16);
                user_data.api_major = (uint8_t)strtol(argv[optind++], NULL, 16);
                user_data.api_minor = (uint8_t)strtol(argv[optind++], NULL, 16);
                user_data.build_id  = (uint8_t)strtol(argv[optind++], NULL, 16);
                user_data.policy    = (uint32_t)strtol(argv[optind++], NULL, 16);
                sev::str_to_array(std::string(argv[optind++]), (uint8_t *)&user_data.digest, sizeof(user_data.digest));
                sev::str_to_array(std::string(argv[optind++]), (uint8_t *)&user_data.mnonce, sizeof(user_data.mnonce));
                sev::str_to_array(std::string(argv[optind++]), (uint8_t *)&user_data.tik,    sizeof(user_data.tik));
                Command cmd(output_folder, verbose_flag);
                cmd_ret = cmd.calc_measurement(&user_data);
                break;
            }
            case 'u': {         // VALIDATE_CERT_CHAIN
                printf("This command is not complete, do not trust the output\n");
                Command cmd(output_folder, verbose_flag);
                cmd_ret = cmd.validate_cert_chain();
                break;
            }
            case 'v': {         // GENERATE_LAUNCH_BLOB
                optind--;   // Can't use option_index because it doesn't account for '-' flags
                if(argc - optind != 1) {
                    printf("Error: Expecting exactly 1 args\n");
                    break;
                }

                uint32_t guest_policy = (uint8_t)strtol(argv[optind++], NULL, 16);
                Command cmd(output_folder, verbose_flag);
                cmd_ret = cmd.generate_launch_blob(guest_policy);
                break;
            }
            case 'w': {         // PACKAGE_SECRET
                Command cmd(output_folder, verbose_flag);
                cmd_ret = cmd.package_secret();
                break;
            }
            case 'T': {         // Run Tests
                Tests test(output_folder, verbose_flag);
                cmd_ret = (test.test_all() == 0); // 0 = fail, 1 = pass
                break;
            }
            case 0:
            case 1 : {
                // Verbose/brief
                break;
            }
            default: {
                fprintf(stderr, "Unrecognised option: -%c\n", optopt);
                break;
            }
        }
    }

    if(cmd_ret == 0) {
        printf("\nCommand Successful\n");
    }
    else if(cmd_ret == 0xFFFF) {
        printf("\nCommand not supported/recognized. Possibly bad formatting\n");
    }
    else {
        printf("\nCommand Unsuccessful: 0x%02x\n", cmd_ret);
    }

    return 0;
}