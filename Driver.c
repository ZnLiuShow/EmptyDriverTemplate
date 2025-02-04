// 完整驱动框架示例（包含设备和IRP处理）
#include <ntddk.h>

extern VOID MyLoadImageNotifyRoutine(
    PUNICODE_STRING FullImageName,
    HANDLE ProcessId,
    PIMAGE_INFO ImageInfo
);

#define MY_IOCTL_CODE CTL_CODE(FILE_DEVICE_UNKNOWN, 0x800, METHOD_BUFFERED, FILE_ANY_ACCESS)

// IRP分发函数
NTSTATUS DispatchCreate(PDEVICE_OBJECT DeviceObject, PIRP Irp) {
    Irp->IoStatus.Status = STATUS_SUCCESS;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);
    return STATUS_SUCCESS;
}

NTSTATUS DispatchControl(PDEVICE_OBJECT DeviceObject, PIRP Irp) {
    PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(Irp);
    PVOID buffer = Irp->AssociatedIrp.SystemBuffer;
    ULONG_PTR dataLength = stack->Parameters.DeviceIoControl.InputBufferLength;

    switch (stack->Parameters.DeviceIoControl.IoControlCode) {
    case MY_IOCTL_CODE:
        DbgPrintEx(DPFLTR_DEFAULT_ID, DPFLTR_INFO_LEVEL, "Received custom IOCTL");
        break;
    default:
        Irp->IoStatus.Status = STATUS_INVALID_DEVICE_REQUEST;
    }

    IoCompleteRequest(Irp, IO_NO_INCREMENT);
    return Irp->IoStatus.Status;
}

VOID DriverUnload(PDRIVER_OBJECT DriverObject) {
    UNICODE_STRING symLink = RTL_CONSTANT_STRING(L"\\DosDevices\\MyDevice");
    IoDeleteSymbolicLink(&symLink);
    IoDeleteDevice(DriverObject->DeviceObject);
    DbgPrintEx(DPFLTR_DEFAULT_ID, DPFLTR_INFO_LEVEL, "Driver unloaded.");
}

NTSTATUS DriverEntry(PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath) {
    // 创建设备
    UNICODE_STRING devName = RTL_CONSTANT_STRING(L"\\Device\\MyDevice");
    PDEVICE_OBJECT DeviceObject;
    IoCreateDevice(DriverObject, 0, &devName, FILE_DEVICE_UNKNOWN, 0, FALSE, &DeviceObject);

    // 创建符号链接
    UNICODE_STRING symLink = RTL_CONSTANT_STRING(L"\\DosDevices\\MyDevice");
    IoCreateSymbolicLink(&symLink, &devName);

    NTSTATUS status = PsSetLoadImageNotifyRoutine(MyLoadImageNotifyRoutine);
    if (!NT_SUCCESS(status)) {
        return status;
    }

    // 注册IRP处理函数
    DriverObject->MajorFunction[IRP_MJ_CREATE] = DispatchCreate;
    DriverObject->MajorFunction[IRP_MJ_CLOSE] = DispatchCreate;
    DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = DispatchControl;
    DriverObject->DriverUnload = DriverUnload;

    DbgPrintEx(DPFLTR_DEFAULT_ID, DPFLTR_INFO_LEVEL, "Driver loaded.");
    return STATUS_SUCCESS;
}