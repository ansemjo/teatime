# Readme

## Notes on the question of voltage regulation

* There's only two components that are somewhat relevant in terms of voltage:
  * the ATtiny; handles 1.7–5.5V per datasheet
  * the LCD; wants 3V per datasheet
* Many of the similar LCD modules are advertised as 3.0–5V compatible.
  * [GoodDisplay's own precautions say that overvoltage will (merely) "shorten its life"](https://www.good-display.com/news/102.html)
  * [Another explainer says that overvoltage usually (only) results in ghosting](https://focuslcds.com/journals/lcd-voltage-inputs-for-lcd-displays-explained/)
* I'll have a diode behind the battery to prevent charging the coin cell in any case.
* The LCD is driven directly by the ATtiny IO pins, so *if* I want to regulate ... I'll need to regulate the input voltage to the ATtiny.
* The ATtiny pins can only handle about VDD + 0.5V, so I'll need either:
  * "Simple" protection with input resistors to limit the current; per § 34.2 of the datasheet it is acceptable to limit the current with $R = \frac{(V_{pin} - (VDD + 0.6))}{I_{Cn}}$, where $I_{Cn} = 1 \text{mA}$
    * How does this interact with the pull-up requirement on I2C pins?
  * "Full" level shifting for any logic pins, i.e. external I2C connectors and programming ports; except UPDI can handle up to 13V regardless of VDD

* Another funky solution would be to omit the regulation and "just" stay within the ATtiny's limits. Then implement the "own voltage" sensing (which is usually used for battery voltage detection; aside: how does that work behind a diode?) to check what voltage is applied on startup. If it's more than 3.3V, use PWM modulation on the IO pins to the display to keep the average at around 3V?

## Driving the LCD

* Aha! These LCDs are so cheap because they don't include and driver electronics. And it's actually not that trivial to drive.
* You *can* apply a constant voltage but that will destroy the LCD over time through ... elctrophoresis? Either way, you need to apply AC by alternating the polarity of each segment during each frame.
* *Do* drive the commons and segments that you *don't* want to be visible, but drive them with equal voltages during both halves of the frame to keep differential at zero. But you still do need to alternate the voltage.
* There's a nice [application note by Atmel: AVR241](https://ww1.microchip.com/downloads/en/AppNotes/doc2569.pdf).
* And [an article by Pacific Display Devices](https://www.pacificdisplay.com/lcd_multiplex_drive.htm).
* And particularly useful is an [application note by ST Microelectronics: AN1447](https://www.st.com/resource/en/application_note/an1447-software-driver-for-4multiplexed-lcd-with-a-standard-st62-stmicroelectronics.pdf). It actually describes how to drive a 4-multiplexed LCD with a simple microcontroller.

## Notes on Components

### The LCD

* Ain't nobody got space for that many pins! I *really* didn't want to use a 40-pin component. Especially because I don't have any microcontrollers that have enough IO ports.
* The 12 pin variant requires time multiplexing but I'm kinda used to that from the other two display projects I built already.

### 32.768 kHz Crystal

* I [picked a crystal from the same series](https://uk.farnell.com/abracon/abs07-120-32-768khz-t/crystal-32-768khz-6pf-3-2-x-1/dp/2467864) that David Johnson-Davies used; but a (to me) more common 12.5 pF loading capacitor value.

* [Per an Electronics StackExchange post](https://electronics.stackexchange.com/questions/39103/selecting-loading-capacitor-values-for-32-khz-crystal), the formula to select (two identical) loading capacitors $C$ is $C_{load} = \frac{C}{2} + C_{parasitic}$, where the parasitic capacitance of the PCB is usually assumed to be around 2.5 pF. Rearranging for the "unknown" yields $C = 2\cdot(C_{load} - C_{parasitic})$ and with $C_{load} = 12.5\,pF$ this gives $C \approx 20\,pF$.
* I chose to use 0603 capacitors because this is a new part in my personal stock, I feel very confident soldering smaller parts than 0805 by now and I'll probably only ever use these capacitors in this exact combination.
