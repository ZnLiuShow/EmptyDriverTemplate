#include <ntddk.h>


VOID MyLoadImageNotifyRoutine(
    PUNICODE_STRING FullImageName,
    HANDLE ProcessId,
    PIMAGE_INFO ImageInfo
) {
    DbgPrintEx(DPFLTR_DEFAULT_ID, DPFLTR_INFO_LEVEL, "ģ�����֪ͨ��\n");
    DbgPrintEx(DPFLTR_DEFAULT_ID, DPFLTR_INFO_LEVEL, "ģ��·����%wZ\n", FullImageName);
    DbgPrintEx(DPFLTR_DEFAULT_ID, DPFLTR_INFO_LEVEL, "����ID��%p\n", ProcessId);
    DbgPrintEx(DPFLTR_DEFAULT_ID, DPFLTR_INFO_LEVEL, "ģ�����ַ��%p\n", ImageInfo->ImageBase);
}