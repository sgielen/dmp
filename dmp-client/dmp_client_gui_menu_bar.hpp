#pragma once

#include "dmp_client_ui_delegate.hpp"
#include "dmp_client_ui_controller_interface.hpp"

#include <QMenuBar>

#include <memory>

class DmpClientGuiMenuBar : public QMenuBar, public DmpClientUiControllerInterface, public DmpClientUiDelegate
{
	Q_OBJECT

	std::shared_ptr<DmpClientInterface> client;

public:
	explicit DmpClientGuiMenuBar(QWidget *parent = 0);

	virtual void set_client(std::shared_ptr<DmpClientInterface> new_client) final;

signals:

public slots:
	void indexFolder();
};
