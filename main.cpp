#include <stdio.h>
#include <stdint.h>
#include <string.h>

void compressIPv6Address(uint8_t *arr, char *str) {
    int longest_zero_sequence_start = -1; // 最长的0序列的起始位置,用于压缩
    int longest_zero_sequence_length = 0; // 最长的0序列的长度 ,用于压缩
    int current_zero_sequence_start = -1; // 当前的0序列的起始位置
    int current_zero_sequence_length = 0; // 当前的0序列的长度
    for (int i = 0; i < 8; i++) { // 遍历8个16位的块
        if (arr[2 * i] == 0 && arr[2 * i + 1] == 0) {
            if (current_zero_sequence_start == -1) {
                current_zero_sequence_start = i;
                current_zero_sequence_length = 1;
            } else {
                current_zero_sequence_length++;
            }
            if (current_zero_sequence_length > longest_zero_sequence_length) {
                longest_zero_sequence_start = current_zero_sequence_start;
                longest_zero_sequence_length = current_zero_sequence_length;
            }
        } else {
            current_zero_sequence_start = -1;
            current_zero_sequence_length = 0;
        }
    }

    int position = 0;
    for (int i = 0; i < 8; i++) { // 遍历8个16位的块
        if (i == longest_zero_sequence_start) {
            if (i == 0) {
                str[position++] = ':';
            }
            str[position++] = ':';
            i += longest_zero_sequence_length - 1;
        } else {
            sprintf(str + position, "%x", (arr[2 * i] << 8) | arr[2 * i + 1]);
            position += strlen(str + position);
            if (i < 7) {
                str[position++] = ':';
            }
        }
    }
    str[position] = '\0';
}

void expandIPv6Address(char *str, uint8_t *arr) {
    int num_colons = 0;
    int double_colon_index = -1;
    for (int i = 0; str[i] != '\0'; i++) {
        if (str[i] == ':') {
            num_colons++; // 统计冒号的个数
            if (str[i + 1] == ':') {
                double_colon_index = num_colons; // 定位双冒号的位置
            }
        }
    }

    int fill_start = double_colon_index != -1 ? double_colon_index : num_colons + 1; // 填充的起始位置
    int fill_length = 8 - num_colons; // 少的冒号,决定了要填充的长度,你自己画图就知道了
    for (int i = 0; i < fill_start; i++) { // 读取前面的数字
        sscanf(str, "%4hx", (uint16_t *)(arr + 2 * i));
        // 反转字节序,我不确定你的机器是大端还是小端,自己改吧
        uint8_t temp = arr[2 * i]; // 2* i是高位,arr的元素是uint8_t,一个字节
        arr[2 * i] = arr[2 * i + 1];
        arr[2 * i + 1] = temp;
        //------------------------------

        str = strchr(str, ':') + 1;// 找到下一个冒号
    }
    for (int i = 0; i < fill_length; i++) { // 填充0
        arr[2 * (fill_start + i)] = 0;
        arr[2 * (fill_start + i) + 1] = 0;
    }
    for (int i = fill_start + fill_length-1; i < 8; i++) { // 读取后面的数字
        sscanf(str, "%4hx", (uint16_t *)(arr + 2 * i));
        // 反转字节序-----------------
        uint8_t temp = arr[2 * i];
        arr[2 * i] = arr[2 * i + 1];
        arr[2 * i + 1] = temp;
        //------------------------
        str = strchr(str, ':') + 1;

    }
}

int main() {
    uint8_t arr[] = {0xfd, 0xbd, 0xdc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x07, 0x00, 0x08, 0x00, 0x41};
    char compressed[40];
    compressIPv6Address(arr, compressed);
    printf("Compressed: %s\n", compressed);

    char *expanded_str = "10:dc00::10:7:8:41";
    uint8_t expanded[16];
    expandIPv6Address(expanded_str, expanded);
    printf("Expanded: ");
    for (int i = 0; i < 16; i++) {
        printf("%02x", expanded[i]);
        if (i % 2 == 1 && i < 15) {
            printf(":");
        }
    }
    printf("\n");

    return 0;
}