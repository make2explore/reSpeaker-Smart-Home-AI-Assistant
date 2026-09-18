from flask import Flask, request, jsonify
import wave
import datetime

app = Flask(__name__)


@app.route('/upload', methods=['POST'])
def upload_audio():
    try:
        # Get audio parameters from headers
        sample_rate = int(request.headers.get('X-Sample-Rate', 16000))
        channels = int(request.headers.get('X-Channels', 2))
        bits_per_sample = int(request.headers.get('X-Bits-Per-Sample', 32))

        # Get raw audio data
        audio_data = request.data

        print(f"Received {len(audio_data)} bytes")
        print(f"Format: {sample_rate}Hz, {channels} channels, {bits_per_sample}-bit")

        # Generate filename with timestamp
        timestamp = datetime.datetime.now().strftime("%Y%m%d_%H%M%S")
        filename = f"recording_{timestamp}.wav"

        # Save as WAV file
        with wave.open(filename, 'wb') as wav_file:
            wav_file.setnchannels(channels)
            wav_file.setsampwidth(bits_per_sample // 8)  # Convert bits to bytes
            wav_file.setframerate(sample_rate)
            wav_file.writeframes(audio_data)

        duration = len(audio_data) / (sample_rate * channels * (bits_per_sample // 8))

        print(f"Saved to {filename} ({duration:.2f} seconds)")

        return jsonify({
            'status': 'success',
            'filename': filename,
            'bytes_received': len(audio_data),
            'duration_seconds': duration
        }), 200

    except Exception as e:
        print(f"Error: {str(e)}")
        return jsonify({'status': 'error', 'message': str(e)}), 500


@app.route('/', methods=['GET'])
def index():
    return "Audio Upload Server Running"


if __name__ == '__main__':
    print("Starting HTTP server on port 8000...")
    print("Waiting for audio uploads from ESP32...")
    app.run(host='0.0.0.0', port=8000, debug=True)
