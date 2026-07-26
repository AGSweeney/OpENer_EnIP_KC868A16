// EXCERPT — source: components/opener/src/ports/ESP32/kc868_a16_application/kc868_a16_application.c
// EVIDENCE: E1 | symbol: ApplicationInitialization | lines: 44-320

#define DEMO_APP_INPUT_ASSEMBLY_NUM   100
#define DEMO_APP_OUTPUT_ASSEMBLY_NUM  150
#define DEMO_APP_CONFIG_ASSEMBLY_NUM  151
#define OUTPUT_ASSEMBLY_SIZE          2
#define CONFIG_ASSEMBLY_SIZE          0
#define INPUT_ASSEMBLY_SIZE           10  /* 2 digital + 4*2 analog */

EipStatus ApplicationInitialization(void) {
  InitializeI2C();
  InitializeAdc();

  CreateAssemblyObject(DEMO_APP_OUTPUT_ASSEMBLY_NUM, s_output_assembly_data,
                       OUTPUT_ASSEMBLY_SIZE);
  CreateAssemblyObject(DEMO_APP_INPUT_ASSEMBLY_NUM, s_input_assembly_data,
                       INPUT_ASSEMBLY_SIZE);
  CreateAssemblyObject(DEMO_APP_CONFIG_ASSEMBLY_NUM, s_config_assembly_data,
                       CONFIG_ASSEMBLY_SIZE);

  ConfigureExclusiveOwnerConnectionPoint(0, 150, 100, 151);
  ConfigureInputOnlyConnectionPoint(0, 150, 100, 151);
  ConfigureListenOnlyConnectionPoint(0, 150, 100, 151);
  CipRunIdleHeaderSetO2T(false);
  CipRunIdleHeaderSetT2O(false);
  return kEipStatusOk;
}
