import socket
import wave
import time

# UDP settings
udp_ip = "0.0.0.0"
udp_port = 12345

# Audio settings (must match ESP32)
SAMPLE_RATE = 16000
CHANNELS = 2
SAMPLE_WIDTH = 4  # 32-bit = 4 bytes

# Expected data size (5 seconds)
EXPECTED_BYTES = 640000

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.bind((udp_ip, udp_port))
sock.settimeout(2.0)  # 2 second timeout

print(f"Listening for audio on {udp_ip}:{udp_port}...")

audio_data = bytearray()
last_packet_time = time.time()

try:
    while True:
        try:
            data, addr = sock.recvfrom(4096)
            if data:
                audio_data.extend(data)
                last_packet_time = time.time()

                # Progress
                if len(audio_data) % 64000 < 4096:
                    print(f"Received {len(audio_data)} bytes ({len(audio_data) / 128000:.1f} seconds)")

        except socket.timeout:
            # No data for 2 seconds - assume transmission complete
            if len(audio_data) > 0:
                print("Timeout - assuming transmission complete")
                break
            else:
                print("Waiting for data...")
                continue

except KeyboardInterrupt:
    print("\nStopped by user")

# Save as WAV file
if len(audio_data) > 0:
    print(f"\nTotal received: {len(audio_data)} bytes")
    print("Saving to output.wav...")

    with wave.open("output.wav", "wb") as wav_file:
        wav_file.setnchannels(CHANNELS)
        wav_file.setsampwidth(SAMPLE_WIDTH)
        wav_file.setframerate(SAMPLE_RATE)
        wav_file.writeframes(bytes(audio_data))

    print("Done! Audio saved to output.wav")
else:
    print("No data received!")

sock.close()
