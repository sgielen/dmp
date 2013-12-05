#include "dmp_client_gui.hpp"

#include "ui_dmp_client_gui.hpp"
#include "message_outputter.hpp"

#include <QFileDialog>

#include <iostream>
#include <string>
#include <condition_variable>

DmpClientGui::DmpClientGui(DmpClient& client_ref, QWidget *parent)
: QMainWindow(parent)
, client(client_ref)
, search_response_model()
, ui(new Ui::DmpClientGui)
{
    client.get_callbacks().set(message::Type::NameRequest, std::function<void(message::NameRequest)>(std::bind(&DmpClient::handle_name_request, &client, std::placeholders::_1))).
                           set(message::Type::SearchRequest, std::function<void(message::SearchRequest)>(std::bind(&DmpClient::handle_search_request, &client, std::placeholders::_1))).
                           set(message::Type::SearchResponse, std::function<void(message::SearchResponse)>(std::bind(&DmpClient::handle_search_response, &client, std::placeholders::_1)));

    client_thread = std::thread(std::bind(&DmpClient::run, &client));

    ui->setupUi(this);

    ui->search_result->setModel(&search_response_model);

    search_response_model.setColumnCount(5);
    search_response_model.setHorizontalHeaderLabels(QStringList({"Artist", "Title", "Album", "Trackno", "User"}));
}


void DmpClientGui::searchBarReturned()
{
    std::string query = ui->search_bar->text().toStdString();
    client.get_callbacks().set(message::Type::SearchResponse, std::function<void(message::SearchResponse)>(std::bind(&DmpClientGui::handle_search_response, this, query, std::placeholders::_1)));

    search_response_model.setRowCount(0);
    client.search(query);
}

void DmpClientGui::handle_search_response(std::string query, message::SearchResponse response)
{
    if(query != response.query) {
        return;
    }

    QString origin = QString::fromStdString(response.origin);
    for(auto entry : response.results)
    {
        QList<QStandardItem*> row({
                                      new QStandardItem(QString::fromStdString(entry.artist)),
                                      new QStandardItem(QString::fromStdString(entry.title)),
                                      new QStandardItem(QString::fromStdString(entry.album)),
                                      new QStandardItem(QString::fromStdString(std::to_string(entry.track))),
                                      new QStandardItem(origin)
            }
        );
        search_response_model.appendRow(row);
    }

    //ToDo: needs to be fixed...
    client.listen_requests();
}

void DmpClientGui::indexFolder()
{
    std::string folder = QFileDialog::getExistingDirectory(this, tr("Select music folder to index")).toStdString();
    if(folder.empty()) {
       return;
    }
    client.index(folder);
}

DmpClientGui::~DmpClientGui()
{
    //Actual destruction.
    auto shutdown = [this]{
        client.stop();
        delete ui;
    };

    //Create a thread to synchronise with the server to reach a destructable state.
    std::thread t;
    auto cb = [&t, this, shutdown](message::ByeAck){
        t = std::thread(shutdown);
    };

    //Register a callback to do the destruction as soon as we receive the ByeAck.
    client.get_callbacks().set(message::Type::ByeAck, cb);
    client.send_bye();

    //A default constructed std::thread isn't joinable so we wait untill it is (and thus has start running).
    while(!t.joinable()) {
        std::this_thread::sleep_for(std::chrono::microseconds(1));
    }
    //Join the destructor thread.
    t.join();

    //Join the client thread.
    client_thread.join();
}
