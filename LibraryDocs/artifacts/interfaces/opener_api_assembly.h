// EXCERPT — source: components/opener/src/opener_api.h
// EVIDENCE: E1 | symbol: CreateAssemblyObject | lines: 593-691

CipInstance *CreateAssemblyObject(const CipInstanceNum instance_number,
                                  EipByte *const data,
                                  const EipUint16 data_length);

void ConfigureExclusiveOwnerConnectionPoint(
  const unsigned int connection_number,
  const unsigned int output_assembly_id,
  const unsigned int input_assembly_id,
  const unsigned int configuration_assembly_id);

void ConfigureInputOnlyConnectionPoint(const unsigned int connection_number,
                                       const unsigned int output_assembly_id,
                                       const unsigned int input_assembly_id,
                                       const unsigned int configuration_assembly_id);

void ConfigureListenOnlyConnectionPoint(const unsigned int connection_number,
                                        const unsigned int output_assembly_id,
                                        const unsigned int input_assembly_id,
                                        const unsigned int configuration_assembly_id);

/* Application callbacks implemented by the product */
EipStatus ApplicationInitialization(void);
EipStatus AfterAssemblyDataReceived(CipInstance *instance);
EipBool8 BeforeAssemblyDataSend(CipInstance *instance);
