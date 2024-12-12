#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

static int processor_info[4];

void manufacturer_info() 
{
    int manufacturer_string[3];
    __asm__ (
        "mov $0x0, %%eax\n"
        "cpuid\n"
        "mov %%ebx, %0\n"
        "mov %%edx, %1\n"
        "mov %%ecx, %2\n"
        : "=r" (manufacturer_string[0]),
          "=r" (manufacturer_string[1]),
          "=r" (manufacturer_string[2])
        :
        : "eax", "ebx", "ecx", "edx"
    );

    printf("Manufacturer string: %s\n", (char*)manufacturer_string);
}

void cpu_information(int eaxValue)
{
    if (eaxValue == 1){
        __asm__("mov $0x80000002, %eax\n");
    }
    else if (eaxValue == 2){
        __asm__("mov $0x80000003, %eax\n");
    }
    else if (eaxValue == 3){
        __asm__("mov $0x80000004, %eax\n");
    }

    __asm__ (
        "cpuid\n"
        "mov %%eax, %0\n"
        "mov %%ebx, %0\n"
        "mov %%ecx, %0\n"
        "mov %%edx, %0\n"
        : "=r" (processor_info[0]),
          "=r" (processor_info[1]),
          "=r" (processor_info[2]),
          "=r" (processor_info[3])
        :
        : "eax", "ebx", "ecx", "edx"
     );

}

void getCPUID() 
{
    printf("Processor information (Model Name): ");
    for (int i = 1; i < 3; i++) {
        cpu_information(i);
    }
    printf("\n");
}

bool is_running_on_vm() 
{
    bool isVM = false;
    __asm__ (
        "xor %%eax, %%eax\n"
        "inc %%eax\n"
        "cpuid\n"
        "bt $0x1f, %%ecx\n"
        "jc UnderVM\n"
        "jmp NotVM\n"
        "UnderVM:\n"
        "movb $1, %0\n"
        "jmp End\n"
        "NotVM:\n"
        "nop\n"
        "End:\n"
        : "+m" (isVM)
        :
        : "eax", "ecx", "cc"
    );

    printf("%s\n", isVM ? "Running in a VM" : "Not Running in a VM");
    return isVM;
}

int main()
{
    manufacturer_info();

    if(is_running_on_vm())
    {
        printf("Terminating the program ...\n");
        exit(1);
    }

    getCPUID();

    return EXIT_SUCCESS;
}
