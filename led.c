#include <18F4550.h>
#fuses HS, NOWDT, NOPROTECT, NOLVP, NOVREGEN, NOPBADEN, NOMCLR
#use delay(clock=4000000)

int8 codigos_anodo_comum[16] = {
   0xC0, 0xF9, 0xA4, 0xB0,
   0x99, 0x92, 0x82, 0xF8,
   0x80, 0x90, 0x88, 0x83,
   0xC6, 0xA1, 0x86, 0x8E
};

// --- Variáveis globais dos clocks ---
int8 contador_10hz = 0;   // conta ciclos para gerar 10Hz
int8 contador_1hz  = 0;   // conta ciclos para gerar 1Hz
int8 estado_10hz   = 0;
int8 estado_1hz    = 0;

// ============================================================
// Cada ciclo do loop = 2ms (1ms por display)
// Para 10Hz ? toggle a cada 50ms  ? 25 ciclos de 2ms
// Para  1Hz ? toggle a cada 500ms ? 250 ciclos de 2ms
// ============================================================

int8 calcular_codigo(int8 valor_codigo) {
   switch(valor_codigo) {
      case 0x0: return codigos_anodo_comum[0];
      case 0x1: return codigos_anodo_comum[1];
      case 0x2: return codigos_anodo_comum[2];
      case 0x3: return codigos_anodo_comum[3];
      case 0x4: return codigos_anodo_comum[4];
      case 0x5: return codigos_anodo_comum[5];
      case 0x6: return codigos_anodo_comum[6];
      case 0x7: return codigos_anodo_comum[7];
      case 0x8: return codigos_anodo_comum[8];
      case 0x9: return codigos_anodo_comum[9];
      case 0xA: return codigos_anodo_comum[10];
      case 0xB: return codigos_anodo_comum[11];
      case 0xC: return codigos_anodo_comum[12];
      case 0xD: return codigos_anodo_comum[13];
      case 0xE: return codigos_anodo_comum[14];
      case 0xF: return codigos_anodo_comum[15];
      default:  return 0xFF;
   }
}

void main() {
   setup_adc_ports(NO_ANALOGS);
   setup_adc(ADC_OFF);

   set_tris_b(0x00);   // PORTB saída (display)
   set_tris_a(0x00);   // PORTA saída (seleção display)
   set_tris_d(0xFF);   // PORTD entrada (chaves)
   set_tris_c(0x00);   // PORTC saída (RC1 e RC2)

   output_a(0x00);
   output_b(0xFF);
   output_c(0x00);

   int8 leitura_d;
   int8 codigo_disp1;
   int8 codigo_disp2;

   while(TRUE) {

      // --- Lê entradas e calcula códigos ---
      leitura_d   = input_d();
      codigo_disp1 = calcular_codigo(leitura_d & 0x0F);
      codigo_disp2 = calcular_codigo((leitura_d >> 4) & 0x0F);

      // --- Multiplexação display 1 ---
      output_a(0x00);           // apaga seleção
      output_b(codigo_disp1);   // carrega display 1
      output_a(0x01);           // liga display 1
      delay_ms(1);

      // --- Multiplexação display 2 ---
      output_a(0x00);           // apaga seleção
      output_b(codigo_disp2);   // carrega display 2
      output_a(0x02);           // liga display 2
      delay_ms(1);

      // --- Gera clock 10Hz em RC1 (toggle a cada 25 ciclos de 2ms = 50ms) ---
      contador_10hz++;
      if (contador_10hz >= 25) {
         contador_10hz = 0;
         estado_10hz = !estado_10hz;
         if (estado_10hz) output_high(PIN_C1);
         else             output_low(PIN_C1);
      }

      // --- Gera clock 1Hz em RC2 (toggle a cada 250 ciclos de 2ms = 500ms) ---
      contador_1hz++;
      if (contador_1hz >= 250) {
         contador_1hz = 0;
         estado_1hz = !estado_1hz;
         if (estado_1hz) output_high(PIN_C2);
         else            output_low(PIN_C2);
      }

   }
}
