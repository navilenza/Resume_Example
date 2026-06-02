import csv
import sys
import os
from rosbag2_py import SequentialReader, StorageOptions, ConverterOptions
from rclpy.serialization import deserialize_message
from sensor_msgs.msg import Imu
from interfaces.msg import ECompass

input_dir = sys.argv[1]

# Create output directory if it does not exist
output_dir = os.path.join(input_dir, "rosbag_csv")
os.makedirs(output_dir, exist_ok=True)

# Find all .db3 files inside the input directory
def find_db3_files(directory):
    db3_files = []
    for root, dirs, files in os.walk(directory):
        for file in files:
            if file.endswith(".db3"):
                db3_files.append(os.path.join(root, file))
    return db3_files

bag_paths = find_db3_files(input_dir)

if not bag_paths:
    print("No rosbag files found.")
    sys.exit(1)

# Function for processing a single bag file
def process_bag(bag_path):
    base = os.path.splitext(os.path.basename(bag_path))[0]
    csv_name = os.path.join(output_dir, f"{base}.csv")

    storage_options = StorageOptions(uri=bag_path, storage_id="sqlite3")
    converter_options = ConverterOptions("", "")
    reader = SequentialReader()
    reader.open(storage_options, converter_options)

    first_timestamp = None  # For relative time starting at 0

    with open(csv_name, "w", newline="") as f:
        writer = csv.writer(f)
        writer.writerow([
            "time_sec",
            "angular_velocity_z",
            "linear_acceleration_x",
            "linear_acceleration_y",
            "heading"
        ])

        while reader.has_next():
            topic, data, t = reader.read_next()

            if first_timestamp is None:
                first_timestamp = t

            t_sec = (t - first_timestamp) * 1e-9  # relative time in seconds

            if topic == "/imu/data":
                msg = deserialize_message(data, Imu)
                writer.writerow([
                    t_sec,
                    msg.angular_velocity.z,
                    msg.linear_acceleration.x,
                    msg.linear_acceleration.y,
                    ""        # empty heading
                ])

            elif topic == "/imu/ecompass":
                msg = deserialize_message(data, ECompass)
                writer.writerow([
                    t_sec,
                    "",       # empty angular_velocity_z
                    "",       # empty linear_acceleration_x
                    "",       # empty linear_acceleration_y
                    msg.heading
                ])

    print(f"Finished {bag_path}")



for bag in bag_paths:
    process_bag(bag)

print(f"\nAll bags processed. CSV files stored in: {output_dir}")
