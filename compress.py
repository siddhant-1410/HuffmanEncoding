import heapq
import os
import struct
from collections import defaultdict, namedtuple

# Define the TreeNode class for the Huffman Tree
class TreeNode:
    def __init__(self, frequency, character=None, left=None, right=None):
        self.frequency = frequency
        self.character = character
        self.left = left
        self.right = right

    def __lt__(self, other):
        return self.frequency < other.frequency

# Build the Huffman Tree
def build_huffman_tree(freq_table):
    priority_queue = [TreeNode(frequency, char) for char, frequency in freq_table.items()]
    heapq.heapify(priority_queue)
    
    while len(priority_queue) > 1:
        left = heapq.heappop(priority_queue)
        right = heapq.heappop(priority_queue)
        merged = TreeNode(left.frequency + right.frequency, left=left, right=right)
        heapq.heappush(priority_queue, merged)
    
    return priority_queue[0]

# Generate Huffman codes
def generate_huffman_codes(root):
    codes = {}
    def traverse(node, code):
        if node.character is not None:
            codes[node.character] = code
        if node.left:
            traverse(node.left, code + '0')
        if node.right:
            traverse(node.right, code + '1')
    
    traverse(root, '')
    return codes

# Read file into buffer
def read_file_into_buffer(file_path):
    with open(file_path, 'rb') as file:
        return file.read()

# Write buffer to file
def write_file_from_buffer(file_path, buffer, mode='wb'):
    with open(file_path, mode) as file:
        file.write(buffer)

# Convert Huffman codes to binary string
def get_huffman_bitstring(buffer, codes):
    output_buffer = ''.join(codes[byte] for byte in buffer)
    padded_bits = (8 - len(output_buffer) % 8) % 8
    if padded_bits > 0:
        output_buffer += '0' * padded_bits
    return output_buffer, padded_bits

# Convert binary string to byte buffer
def get_buffer_from_string(bitstring):
    byte_array = bytearray()
    for i in range(0, len(bitstring), 8):
        byte_array.append(int(bitstring[i:i + 8], 2))
    return byte_array

# Write header to compressed file
def write_header(file_path, codes, padded_bits):
    with open(file_path, 'wb') as file:
        file.write(struct.pack('I', padded_bits))
        file.write(struct.pack('I', len(codes)))
        for char, code in codes.items():
            file.write(struct.pack('B', char))
            file.write(struct.pack('I', len(code)))
            file.write(code.encode())

# Compress file
def compress_file(input_path, output_path):
    buffer = read_file_into_buffer(input_path)
    freq_table = defaultdict(int)
    
    for byte in buffer:
        freq_table[byte] += 1
    
    root = build_huffman_tree(freq_table)
    codes = generate_huffman_codes(root)
    
    bitstring, padded_bits = get_huffman_bitstring(buffer, codes)
    byte_buffer = get_buffer_from_string(bitstring)
    
    write_header(output_path, codes, padded_bits)
    write_file_from_buffer(output_path, byte_buffer, mode='ab')

if __name__ == "__main__":
    input_file = "file.txt"
    compressed_file = "test1.txt"
    
    compress_file(input_file, compressed_file)
    
    print("Compression completed successfully!")
