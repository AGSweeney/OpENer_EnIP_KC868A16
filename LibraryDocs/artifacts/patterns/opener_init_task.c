// EXCERPT — source: components/opener/src/ports/ESP32/opener.c
// EVIDENCE: E1 | symbol: opener_init | lines: 23-134

#define OPENER_THREAD_PRIO   5
#define OPENER_STACK_SIZE    8192

void opener_init(struct netif *netif) {
  /* Mutex-guarded single init; requires link up */
  if (!IfaceLinkIsUp(netif)) {
    g_end_stack = 1;
    return;
  }

  DoublyLinkedListInitialize(&connection_list, ...);
  SetDeviceSerialNumber(123456789);
  EipUint16 unique_connection_id = (EipUint16)(esp_random() & 0xFFFF);
  CipStackInit(unique_connection_id);

  InsertGetSetCallback(GetCipClass(kCipTcpIpInterfaceClassCode),
                       NvTcpipSetCallback, kNvDataFunc);
  NvTcpipLoad(&g_tcpip);
  NetworkHandlerInitialize();

  /* Pin OpENer cyclic handler to Core 0 (same as lwIP TCP/IP task) */
  xTaskCreatePinnedToCore(opener_thread, "OpENer", OPENER_STACK_SIZE,
                          netif, OPENER_THREAD_PRIO, &opener_task_handle, 0);
}
