import numpy as np
import pandas as pd
import tensorflow as tf
import joblib
import os

# Configuration
SCALER_PATH = 'scaler_x.pkl'
TEMP_MODEL_PATH = 'temperature_model.tflite'
HUM_MODEL_PATH = 'humidity_model.tflite'

def load_interpreter(model_path):
    """Loads a TFLite model and allocates tensors."""
    if not os.path.exists(model_path):
        raise FileNotFoundError(f"Model not found at {model_path}")
        
    interpreter = tf.lite.Interpreter(model_path=model_path)
    interpreter.allocate_tensors()
    return interpreter

def predict(interpreter, input_val):
    """Runs inference on a single input value."""
    # Get input and output details
    input_details = interpreter.get_input_details()
    output_details = interpreter.get_output_details()
    
    # Check expected shape (Batch size, Features)
    # The model expects float32
    input_data = np.array(input_val, dtype=np.float32)
    
    # Set the tensor
    interpreter.set_tensor(input_details[0]['index'], input_data)
    
    # Run
    interpreter.invoke()
    
    # Get result
    output_data = interpreter.get_tensor(output_details[0]['index'])
    return output_data[0][0]

def main():
    # 1. Load the Scaler
    # We must use the EXACT same scaler used during training
    if not os.path.exists(SCALER_PATH):
        print(f"Error: {SCALER_PATH} not found. Run the training script first.")
        return
    
    print("Loading scaler...")
    scaler = joblib.load(SCALER_PATH)
    
    # 2. Load Models
    print("Loading TFLite models...")
    temp_interpreter = load_interpreter(TEMP_MODEL_PATH)
    hum_interpreter = load_interpreter(HUM_MODEL_PATH)
    
    # 3. Define Test Input
    # Using the first row of your provided data
    test_timestamp = "2025-11-27 10:21:48"
    print(f"\n--- Inference Request ---")
    print(f"Input Timestamp: {test_timestamp}")
    
    # 4. Preprocess Input
    # A. Convert String -> Unix Timestamp (Seconds)
    ts = pd.to_datetime([test_timestamp])
    unix_time = ts.astype('int64') / 10**9
    
    # B. Scale the input
    # Reshape to (-1, 1) because scaler expects a 2D array [[value]]
    unix_reshaped = unix_time.values.reshape(-1, 1)
    scaled_input = scaler.transform(unix_reshaped)
    
    print(f"Unix Time:       {unix_time[0]}")
    print(f"Scaled Input:    {scaled_input[0][0]:.6f} (This goes into the model)")
    
    # 5. Run Prediction
    temp_pred = predict(temp_interpreter, scaled_input)
    hum_pred = predict(hum_interpreter, scaled_input)
    
    # 6. Display Results
    print(f"\n--- Results ---")
    print(f"Predicted Temperature: {temp_pred:.2f}°C")
    print(f"Predicted Humidity:    {hum_pred:.2f}%")

if __name__ == "__main__":
    main()