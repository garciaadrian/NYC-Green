#include "nyc_green_view.h"
#include "nyc_green_model.h"

#include <cpprest/http_client.h>
#include <cpprest/filestream.h>
#include "nyc_green_model.h"
#include <nlohmann/json.hpp>
#include <locale>
#include <codecvt>

#include <qmessagebox.h>

struct coordinate {
    std::string latitude;
    std::string longitude;
};

void NYC_Green::onWattSlider() {
    QString watts;
    watts += QString::number(this->ui.watt_slider->value());
    watts += " watts";
    this->ui.watt_label->setText(watts);
}

void NYC_Green::onSpaceSlider() {
    QString text;
    text += QString::number(ui.space_slider->value());
    text += " square feet";
    ui.space_label->setText(text);
}

void NYC_Green::onAbout() {
    std::wstring attributions = L"© 2018 Mapbox and its suppliers. All rights reserved. Use of this data is subject to the Mapbox Terms of Service.";
    QMessageBox msg;
    msg.setText("Attributions");
    msg.setInformativeText(QString::fromStdWString(attributions));
    msg.setStandardButtons(QMessageBox::Ok);
    msg.setDefaultButton(QMessageBox::Abort);
    msg.setIcon(QMessageBox::Information);
    int ret2 = msg.exec();
}

NYC_Green::NYC_Green(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
    ui.watt_slider->setTracking(true);
    connect(this->ui.button_about, SIGNAL(clicked()), this, SLOT(onAbout()));
    connect(this->ui.submitButton, SIGNAL(clicked()), this, SLOT(onSubmit()));
    connect(this->ui.watt_slider, SIGNAL(valueChanged(int)), this, SLOT(onWattSlider()));
    connect(this->ui.space_slider, SIGNAL(valueChanged(int)), this, SLOT(onSpaceSlider()));
}

coordinate get_geocode(std::wstring address) {
    auto http_path = uri::encode_uri(address, uri::components::path);
    http_path += L".json";

    GeocodeRequest req(U("https://api.mapbox.com"), U("/geocoding/v5/mapbox.places/"));
    req.append_path(http_path);
    req.request();

    nlohmann::json j2;
    if (req.is_successful()) {
        j2 = req.get_json();
    }


    // geocode API will return an array of possible results for a given entry. 0th element is the most likely match.
    nlohmann::json addresses = nlohmann::json::array();
    addresses = j2["features"];

    auto& address_match = addresses[0];

    coordinate coords;

    nlohmann::json long_json = address_match["geometry"]["coordinates"][0].dump();
    nlohmann::json lat_json = address_match["geometry"]["coordinates"][1].dump();

    coords.longitude = long_json.dump();
    coords.latitude = lat_json.dump();

    // removes backwards slash from beginning and end of string
    coords.longitude = coords.longitude.substr(1, coords.longitude.size() - 2);
    coords.latitude = coords.latitude.substr(1, coords.latitude.size() - 2);

    return coords;
}

float get_ghi(coordinate coords) {

    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> wide_converter;
    std::wstring wide_latitude = wide_converter.from_bytes(coords.latitude);
    std::wstring wide_longitude = wide_converter.from_bytes(coords.longitude);

    NRELRequest req(U("https://developer.nrel.gov"), U("/api/solar/solar_resource/v1.json/"));
    req.append_query(U("lat"), wide_latitude);
    req.append_query(U("lon"), wide_longitude);

    req.request();

    nlohmann::json json;
    if (req.is_successful()) {
        json = req.get_json();
    }
    else {
        throw std::runtime_error("Unsuccesful request to NREL API.");
    }


    float annual_ghi = json["outputs"]["avg_ghi"]["annual"];

    return annual_ghi;
}

float get_energy_price(const coordinate& coords) {
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> wide_converter;
    std::wstring wide_latitude = wide_converter.from_bytes(coords.latitude);
    std::wstring wide_longitude = wide_converter.from_bytes(coords.longitude);

    NRELRequest req(U("https://developer.nrel.gov"), U("/api/utility_rates/v3.json/"));
    req.append_query(U("lat"), wide_latitude);
    req.append_query(U("lon"), wide_longitude);
    req.append_query(U("limit"), U("3"));

    req.request();

    nlohmann::json json;
    if (req.is_successful()) {
        json = req.get_json();
    }
    else {
        // throw here
        return 0.0f;
    }

    float utility_rate = json["outputs"]["residential"];


    return utility_rate;
}

void NYC_Green::onSubmit() {
    try {

        QString address = ui.lineEdit->text();
        if (address.length() == 0) {
            return;
        }
        auto ret = get_geocode(address.toStdWString());
        float annual_ghi = get_ghi(ret);
        float utility_rate = get_energy_price(ret);

        QString output;
        output += "ADDRESS: "; output += address; output += "\n";
        float kilowatt_hour_annual = ((ui.watt_slider->value() * (ui.space_slider->value() / 17.55) * annual_ghi) / 1000) * 365;
        float savings = kilowatt_hour_annual * utility_rate;
        output += "ANNUAL kWh: "; output += QString::number(kilowatt_hour_annual);
        output += "\nSAVINGS PER YEAR: $"; output += QString::number(savings);
        ui.log_output->setPlainText(output);
    }
    catch (const std::exception& e) {
        qWarning() << e.what();
        QMessageBox msg;
        msg.setText("There has been an error processing your request!");
        msg.setInformativeText(e.what());
        msg.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        msg.setDefaultButton(QMessageBox::Save);
        msg.setIcon(QMessageBox::Critical);
        int ret2 = msg.exec();   
    }
}