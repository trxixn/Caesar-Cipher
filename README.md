# Caesar Cipher Frequency Analysis

A C program that implements the Caesar cipher encryption/decryption and automatically breaks it using frequency analysis. The system tests all possible shifts (0-25) and computes how closely the resulting decrypted text's letter distribution matches standard English frequencies using three distance metrics:

- Chi-Squared (χ²) Distance
- Cosine Distance
- Euclidean Distance

## Features

- Encrypt and decrypt text using the Caesar cipher
- Automatic cipher breaking using frequency analysis
- Multiple distance metrics for analysis
- Support for various text lengths and structures

## Implementation Details

### Testing Procedure

1. **Text Length Variations**
   - Short Texts (10-20 characters): Simple sentences or phrases
   - Long Texts (200+ characters): Paragraphs from articles or classic literature

2. **Random Shifts**
   - Applied shifts like 5, 13, 19, or 23
   - Automatic decryption using three distance metrics

3. **Special Text Structures**
   - Highly Repetitive Text
   - Unusual Letter Frequencies

### Results

#### Short Texts (10-20 characters)
- Chi-Squared: Often correct, but can be off by 1-2 shift values
- Cosine: Performs reasonably but may have ties
- Euclidean: Similar to Chi-Squared, struggles with very short texts

#### Long Texts (200+ characters)
- Chi-Squared: Most consistent performance
- Cosine: Typically correct, sometimes second-best
- Euclidean: Often matches Chi-Squared's success

#### Edge Cases
- High Repetition / Limited Variety: All methods may struggle
- Missing Common Letters: Less reliable analysis
- Very Short Text (<10 characters): All metrics can fail

## Example Results

| Text & Shift | Length | Chi-Squared | Cosine | Euclidean |
|--------------|---------|-------------|---------|-----------|
| HELLO WORLD (Shift 5) | 11 chars | Correct (5) | 2nd best guess | Off by 1 |
| Paragraph (Shift 13) | 300+ chars | Correct (13) | Correct (13) | Correct (13) |
| AAAA BBBB CCCC (3) | 14 chars | Correct (3) | Incorrect (8) | Correct (3) |

## Conclusions

- Text length and distribution are key factors for successful cipher breaking
- Chi-Squared is typically most reliable for standard English
- Euclidean performs well for normal texts
- Cosine is effective but more prone to ties
- All metrics work well with longer texts
- Very short or unusual texts can cause all methods to fail

## Author

Traian Sfarghiu

## License

This project is open source and available under the MIT License. 