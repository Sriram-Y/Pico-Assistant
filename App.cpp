#include "App.hpp"

#define GPIO_PIN 4

int main()
{
    if (gpioInitialise() < 0)
    {
        std::cerr << "Failed to init pigpio\n";
        return 1;
    }

    std::unique_ptr<App> app = std::make_unique<App>();
    const std::string speechCommand = "what is the humidity";
    const bool res = app->get_command_result(app->parse_command(speechCommand));

    if (!res)
    {
        std::cout << "Failed to get desired data!" << std::endl;
    }

    gpioTerminate();
    return 0;
}

bool App::get_command_result(uint8_t cmd)
{
    bool res = false;
    switch (cmd)
    {
    case 0:
        res = predict_temperature();
        break;
    case 1:
        res = predict_humidity();
        break;
    case 2:
        res = read_temperature();
        std::cout << "Temperature: " << static_cast<int>(temperature_) << " C" << std::endl;
        break;
    case 3:
        res = read_humidity();
        std::cout << "Humidity: " << static_cast<int>(humidity_) << " %" << std::endl;
        break;
    default:
        std::cout << "Unrecognized command or no command" << std::endl;
        break;
    }

    return res;
}

uint8_t App::parse_command(std::string speechCommand)
{
    // if "tomorrow" & "temperature" is in the string return 0
    // if "tomorrow" & "humidity" is in the string return 1
    // if "temperature" found in speechCommand return 2
    // if "humidity" found in speechCommand return 3

    if (speechCommand.find("tomorrow") != std::string::npos &&
        speechCommand.find("temperature") != std::string::npos)
    {
        return 0;
    }
    else if (speechCommand.find("tomorrow") != std::string::npos &&
             speechCommand.find("humidity") != std::string::npos)
    {
        return 1;
    }
    else if (speechCommand.find("temperature") != std::string::npos)
    {
        return 2;
    }
    else if (speechCommand.find("humidity") != std::string::npos)
    {
        return 3;
    }
    else
    {
        return 255;
    }
}

bool App::read_temperature()
{
    std::unique_ptr<ReadSensor> sensor = std::make_unique<ReadSensor>(GPIO_PIN);
    if (!sensor->read())
    {
        return false;
    }

    temperature_ = sensor->getTemperatureC();
    return true;
}

bool App::read_humidity()
{
    std::unique_ptr<ReadSensor> sensor = std::make_unique<ReadSensor>(GPIO_PIN);

    if (!sensor->read())
    {
        return false;
    }

    humidity_ = sensor->getHumidity();
    return true;
}

bool App::predict_temperature()
{
    return true;
}

bool App::predict_humidity()
{
    return true;
}
