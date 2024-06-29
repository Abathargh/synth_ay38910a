"""
Simple script used to generate the coefficients to be used to play notes in
the equal temperament system with the AY38910a programmable sound generator.

We can generate the equal temperament frequencies by using a known frequency
as the starting point - in this case A4 = 440 Hz - by leveraging a known
formula:
            fn = f0 * a^n, a = 2^(1/12)
            f0 = f_A4 = 440 Hz

Where n is the number of half steps away from the center note (a negative n
denotes a note 'n' half steps lower than the center note, while a positive 'n'
denotes a note 'n' half steps higher).

Once the frequencies are generated, we can compute the actual values that must
be written onto the AY38910a ports in order to play the respective note.

The frequency of a note is given by:
        fn = f_clk / (16 * mask)
        mask = 1/16 * (f_clk / fn), f_clk = 2 MHz


References:
    - Equations for the Frequency Table:
        https://pages.mtu.edu/~suits/NoteFreqCalcs.html
"""

octave = 12                   # Notes in an octave
freq_a4 = 440                 # Fourth octave A note (Hz)
f_clk = 2_000_000             # Ay3910a clock signal (Hz)
a4_idx = 0                    # A4 is the 0th note in
b4_idx = 2                    # B4 is the 2nd note
b0_n = b4_idx - (4 * octave)  # B0 index (lowest note)
b8_n = b4_idx + (4 * octave)  # B8 index (highest note)


def freq(n: int) -> float:
    return freq_a4 * (2 ** (1 / 12)) ** n


def mask(f: float) -> int:
    return int(f_clk // (16 * f))


magic_values = [mask(freq(i)) for i in range(b0_n, b8_n+1)]

print("static const unsigned int magic_notes[] = {")
print(f"\t{magic_values[0]}")
start = 1
for i in range(1, len(magic_values), octave):
    row = [str(e) for e in magic_values[start:start+octave]]
    print("\t" + ", ".join(row))
    start += octave
print("};")
