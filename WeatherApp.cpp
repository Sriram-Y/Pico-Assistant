#include "WeatherApp.hpp"

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        std::cerr << "Usage: " << argv[0] << " \"<speech command>\"" << std::endl;
        return 1;
    }

    if (gpioInitialise() < 0)
    {
        std::cerr << "Failed to init pigpio\n";
        return 1;
    }

    const std::string speechCommand = argv[1];
    std::unique_ptr<WeatherApp> app = std::make_unique<WeatherApp>();
    const bool res = app->get_command_result(app->parse_command(speechCommand));

    if (!res)
    {
        std::cout << "Failed to get desired data!" << std::endl;
    }

    gpioTerminate();
    return 0;
}

bool WeatherApp::get_command_result(uint8_t cmd)
{
    bool res = false;
    std::string ts = get_time_tomorrow();
    switch (cmd)
    {
    case PREDICT_TEMP_CMD:
        res = predict_temperature();
        if (!res)
        {
            std::cout << "Failed to predict temperature!" << std::endl;
            return false;
        }

        std::cout << "Tempearture on " << ts << " will be: " << static_cast<int>(temperature_) << " C" << std::endl;

        break;
    case PREDICT_HUM_CMD:
        res = predict_humidity();
        if (!res)
        {
            std::cout << "Failed to predict humidity!" << std::endl;
            return false;
        }

        std::cout << "Humidity on " << ts << " will be: " << static_cast<int>(humidity_) << " %" << std::endl;

        break;
    case READ_TEMP_CMD:
        res = read_temperature();
        if (!res)
        {
            std::cout << "Failed to read temperature!" << std::endl;
            return false;
        }

        std::cout << "Temperature: " << static_cast<int>(temperature_) << " C" << std::endl;

        break;
    case READ_HUM_CMD:
        res = read_humidity();
        if (!res)
        {
            std::cout << "Failed to read humidity!" << std::endl;
            return false;
        }

        std::cout << "Humidity: " << static_cast<int>(humidity_) << " %" << std::endl;

        break;
    default:
        std::cout << "Unrecognized command or no command" << std::endl;
        break;
    }

    return res;
}

uint8_t WeatherApp::parse_command(std::string speechCommand)
{
    // if "tomorrow" & "temperature" is in the string return 0
    // if "tomorrow" & "humidity" is in the string return 1
    // if "temperature" found in speechCommand return 2
    // if "humidity" found in speechCommand return 3

    if (speechCommand.find("tomorrow") != std::string::npos &&
        speechCommand.find("temperature") != std::string::npos)
    {
        return PREDICT_TEMP_CMD;
    }
    else if (speechCommand.find("tomorrow") != std::string::npos &&
             speechCommand.find("humidity") != std::string::npos)
    {
        return PREDICT_HUM_CMD;
    }
    else if (speechCommand.find("temperature") != std::string::npos)
    {
        return READ_TEMP_CMD;
    }
    else if (speechCommand.find("humidity") != std::string::npos)
    {
        return READ_HUM_CMD;
    }
    else
    {
        return INVALID_CMD;
    }
}

bool WeatherApp::read_temperature()
{
    std::unique_ptr<ReadSensor> sensor = std::make_unique<ReadSensor>(GPIO_PIN);
    if (!sensor->read())
    {
        return false;
    }

    temperature_ = sensor->getTemperatureC();
    return true;
}

bool WeatherApp::read_humidity()
{
    std::unique_ptr<ReadSensor> sensor = std::make_unique<ReadSensor>(GPIO_PIN);

    if (!sensor->read())
    {
        return false;
    }

    humidity_ = sensor->getHumidity();
    return true;
}

bool WeatherApp::predict_temperature()
{   
    const std::string ts = get_time_tomorrow();

    temperature_ = temperatureInference_->predictTemperature(ts);
    return true;
}

bool WeatherApp::predict_humidity()
{
    const std::string ts = get_time_tomorrow();

    humidity_ = humidityInference_->predictHumidity(ts);
    return true;
}

std::string WeatherApp::get_time_tomorrow()
{
    auto now = std::chrono::system_clock::now();
    auto tomorrow = now + std::chrono::hours(24);
    std::time_t t = std::chrono::system_clock::to_time_t(tomorrow);
    std::tm tm = *std::localtime(&t);

    char buffer[32];
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &tm);
    std::string ts(buffer);

    return ts;
}
