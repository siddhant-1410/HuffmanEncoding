import os
import struct
from collections import defaultdict

# Convert an integer to an 8-bit binary string
def to_binary(a):
    return f'{a:08b}'

# Read file into buffer
def read_file_into_buffer(file_path):
    with open(file_path, 'rb') as file:
        return file.read()

# Write buffer to file
def write_file_from_buffer(file_path, buffer):
    with open(file_path, 'wb') as file:
        file.write(buffer)

# Convert buffer to bitstring
def get_string_from_buffer(buffer):
    return ''.join(to_binary(byte) for byte in buffer)

# Decode the bitstring using Huffman codes
def get_decoded_buffer(bitstring, codes, padded_bits):
    reverse_codes = {code: char for char, code in codes.items()}
    bit = ''
    output_buffer = bytearray()
    
    for bit_char in bitstring[:-padded_bits]:
        bit += bit_char
        if bit in reverse_codes:
            output_buffer.append(reverse_codes[bit])
            bit = ''
    
    return output_buffer

# Read header from buffer to retrieve Huffman codes
def read_header(buffer):
    padded_bits = struct.unpack('I', buffer[:4])[0]
    buffer = buffer[4:]
    size = struct.unpack('I', buffer[:4])[0]
    buffer = buffer[4:]
    
    codes = {}
    for _ in range(size):
        key = buffer[0]
        buffer = buffer[1:]
        len_code = struct.unpack('I', buffer[:4])[0]
        buffer = buffer[4:]
        code = buffer[:len_code].decode()
        buffer = buffer[len_code:]
        codes[key] = code
    
    return codes, padded_bits, buffer

# Decompress file
def decompress_file(input_path, output_path):
    file_buffer = read_file_into_buffer(input_path)
    
    codes, padded_bits, file_buffer = read_header(file_buffer)
    
    file_bitstring = get_string_from_buffer(file_buffer)
    
    output_buffer = get_decoded_buffer(file_bitstring, codes, padded_bits)
    
    write_file_from_buffer(output_path, output_buffer)

if __name__ == "__main__":
    compressed_file = "test1.txt"
    decompressed_file = "decoded.txt"
    
    decompress_file(compressed_file, decompressed_file)
    
    print("Decompression completed successfully!")
