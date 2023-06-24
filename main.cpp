#include <stdio.h>
#include <stdint.h>
#include <string.h>

void compressIPv6Address(uint8_t *arr, char *str) {
    int longest_zero_sequence_start = -1; // ���0���е���ʼλ��,����ѹ��
    int longest_zero_sequence_length = 0; // ���0���еĳ��� ,����ѹ��
    int current_zero_sequence_start = -1; // ��ǰ��0���е���ʼλ��
    int current_zero_sequence_length = 0; // ��ǰ��0���еĳ���
    for (int i = 0; i < 8; i++) { // ����8��16λ�Ŀ�
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
    for (int i = 0; i < 8; i++) { // ����8��16λ�Ŀ�
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
            num_colons++; // ͳ��ð�ŵĸ���
            if (str[i + 1] == ':') {
                double_colon_index = num_colons; // ��λ˫ð�ŵ�λ��
            }
        }
    }

    int fill_start = double_colon_index != -1 ? double_colon_index : num_colons + 1; // ������ʼλ��
    int fill_length = 8 - num_colons; // �ٵ�ð��,������Ҫ���ĳ���,���Լ���ͼ��֪����
    for (int i = 0; i < fill_start; i++) { // ��ȡǰ�������
        sscanf(str, "%4hx", (uint16_t *)(arr + 2 * i));
        // ��ת�ֽ���,�Ҳ�ȷ����Ļ����Ǵ�˻���С��,�Լ��İ�
        uint8_t temp = arr[2 * i]; // 2* i�Ǹ�λ,arr��Ԫ����uint8_t,һ���ֽ�
        arr[2 * i] = arr[2 * i + 1];
        arr[2 * i + 1] = temp;
        //------------------------------

        str = strchr(str, ':') + 1;// �ҵ���һ��ð��
    }
    for (int i = 0; i < fill_length; i++) { // ���0
        arr[2 * (fill_start + i)] = 0;
        arr[2 * (fill_start + i) + 1] = 0;
    }
    for (int i = fill_start + fill_length-1; i < 8; i++) { // ��ȡ���������
        sscanf(str, "%4hx", (uint16_t *)(arr + 2 * i));
        // ��ת�ֽ���-----------------
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