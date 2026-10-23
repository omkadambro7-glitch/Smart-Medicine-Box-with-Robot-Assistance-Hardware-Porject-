#ifndef ARDUINO_COMPAT_H
#define ARDUINO_COMPAT_H

/**
 * ============================================================================
 * Arduino & IDE Language Server Compatibility Layer
 * ============================================================================
 * When compiled inside the Arduino IDE or via Arduino CLI/ESP-IDF, the real
 * hardware-native <Arduino.h> is included automatically.
 * When viewed in desktop IDEs (VS Code / Antigravity / clangd), this header
 * provides fallback type stubs so that code navigation, syntax highlighting,
 * and static analysis work smoothly without "file not found" errors.
 */

#if defined(__has_include)
  #if __has_include(<Arduino.h>)
    #include <Arduino.h>
  #else
    // Desktop IDE Language Server (clangd) fallback declarations
    #include <stdint.h>
    #include <stddef.h>
    #include <stdbool.h>

    #define HIGH 1
    #define LOW 0
    #define OUTPUT 1
    #define INPUT 0

    void pinMode(uint8_t pin, uint8_t mode);
    void digitalWrite(uint8_t pin, uint8_t val);
    int digitalRead(uint8_t pin);
    unsigned long millis();
    void delay(unsigned long ms);
    void yield();
    long map(long x, long in_min, long in_max, long out_min, long out_max);

    void ledcSetup(uint8_t chan, uint32_t freq, uint8_t res);
    void ledcAttachPin(uint8_t pin, uint8_t chan);
    void ledcWrite(uint8_t chan, uint32_t duty);
    void ledcAttach(uint8_t pin, uint32_t freq, uint8_t res);

    class String {
    public:
      String();
      String(const char* s);
      size_t length() const;
      char charAt(size_t index) const;
      void trim();
      String substring(size_t start) const;
      int toInt() const;
      const char* c_str() const;
    };

    class Stream {
    public:
      void print(const char* s);
      void print(int n);
      void print(long n);
      void print(unsigned long n);
      void println(const char* s = "");
      void println(int n);
      void println(long n);
      void println(unsigned long n);
      int available();
      String readStringUntil(char terminator);
      void begin(unsigned long baud);
    };

    extern Stream Serial;
  #endif
#else
  #include <Arduino.h>
#endif

#endif // ARDUINO_COMPAT_H
