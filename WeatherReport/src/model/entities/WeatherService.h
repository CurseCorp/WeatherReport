#pragma once
#include "../entities/WeatherData.h"
#include <string>
#include <algorithm>

class WeatherService {
public:
    WeatherData fetchCurrentWeather(const std::string& city) {

        std::string lowerCity = city;
        std::transform(lowerCity.begin(), lowerCity.end(), lowerCity.begin(), ::tolower);

        WeatherData data;


        data.cityName = city;
        data.temperature = 22.0;
        data.humidity = 50;
        data.description = "Clear sky";

        // Заглушка 1: Лондон (Дождь)
        if (lowerCity == "london" || lowerCity == "лондон") {
            data.temperature = 14.5;
            data.humidity = 92;
            data.description = "Heavy rain";
        }
        // Заглушка 2: Дубай (Жара)
        else if (lowerCity == "dubai" || lowerCity == "дубай") {
            data.temperature = 41.2;
            data.humidity = 20;
            data.description = "Sunny and hot";
        }
        // Заглушка 3: Якутск (Мороз)
        else if (lowerCity == "yakutsk" || lowerCity == "якутск") {
            data.temperature = -35.0;
            data.humidity = 75;
            data.description = "Snow and freezing fog";
        }
        // Заглушка 4: Сиэтл (Облачно)
        else if (lowerCity == "seattle" || lowerCity == "сиэтл") {
            data.temperature = 16.0;
            data.humidity = 80;
            data.description = "Mostly cloudy";
        }

        return data;
    }
};
