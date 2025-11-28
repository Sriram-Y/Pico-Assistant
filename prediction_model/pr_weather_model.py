import pandas as pd
import numpy as np
import tensorflow as tf
from sklearn.model_selection import train_test_split
from sklearn.preprocessing import StandardScaler
import joblib 

# Clear any background sessions
tf.keras.backend.clear_session()

# 1. Load the Data
try:
    df = pd.read_csv('sensor_log.csv')
    print("Data loaded successfully.")
except FileNotFoundError:
    print("CSV not found. Generating dummy data...")
    dates = pd.date_range(start='2025-09-04 20:47:00', periods=100, freq='S')
    df = pd.DataFrame({
        'timestamp': dates,
        'temperature_c': 20 + 0.5 * np.sin(np.linspace(0, 10, 100)) + np.random.normal(0, 0.1, 100),
        'humidity_percent': 50 + 0.5 * np.cos(np.linspace(0, 10, 100)) + np.random.normal(0, 0.1, 100)
    })

# 2. Preprocessing
df['timestamp'] = pd.to_datetime(df['timestamp'])
df['unix_time'] = df['timestamp'].astype(int) / 10**9

X = df[['unix_time']].values
y_temp = df['temperature_c'].values
y_hum = df['humidity_percent'].values

scaler_x = StandardScaler()
X_scaled = scaler_x.fit_transform(X)

# CAST TO FLOAT32 (Mandatory for TFLite)
X_scaled = X_scaled.astype(np.float32)
y_temp = y_temp.astype(np.float32)
y_hum = y_hum.astype(np.float32)

joblib.dump(scaler_x, 'scaler_x.pkl')

X_train, X_test, y_temp_train, y_temp_test, y_hum_train, y_hum_test = train_test_split(
    X_scaled, y_temp, y_hum, test_size=0.2, random_state=42
)

# 3. Define the Model
def create_polynomial_model():
    model = tf.keras.Sequential([
        tf.keras.layers.Input(shape=(1,), name='input_layer'),
        tf.keras.layers.Dense(64, activation='relu'),
        tf.keras.layers.Dense(64, activation='relu'),
        tf.keras.layers.Dense(1)
    ])
    model.compile(optimizer='adam', loss='mse', metrics=['mae'])
    return model

# 4. Train
print("\n--- Training Temperature Model ---")
temp_model = create_polynomial_model()
temp_model.fit(X_train, y_temp_train, epochs=10, batch_size=10, verbose=0)

print("\n--- Training Humidity Model ---")
hum_model = create_polynomial_model()
hum_model.fit(X_train, y_hum_train, epochs=10, batch_size=10, verbose=0)

# 5. Convert using Concrete Functions (THE FIX)
def convert_via_concrete_function(model, filename):
    # A. Define a specific execution function
    @tf.function
    def inference_func(x):
        return model(x)
    
    # B. Generate the Concrete Function
    # We explicitly tell TF: "Expect an input of shape [1, 1] and type float32"
    # This shape [1, 1] corresponds to [Batch_Size, Features]
    concrete_func = inference_func.get_concrete_function(
        tf.TensorSpec(shape=[1, 1], dtype=tf.float32)
    )
    
    # C. Convert using the concrete function list
    converter = tf.lite.TFLiteConverter.from_concrete_functions([concrete_func])
    tflite_model = converter.convert()
    
    # D. Save
    with open(filename, 'wb') as f:
        f.write(tflite_model)
    print(f"Success! Model saved to {filename}")

# Run conversion
print("\n--- Converting Models ---")
convert_via_concrete_function(temp_model, 'temperature_model.tflite')
convert_via_concrete_function(hum_model, 'humidity_model.tflite')

print("\nProcess Complete. Models and scaler saved.")