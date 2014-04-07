#pragma once

#include "qt_adapter.hpp"
#include "radio_list_model.hpp"

#include <QAbstractListModel>

#include <memory>

class RadioListModelQtAdapter : public QtAdapter<RadioListModel>, public QAbstractListModel
{
public:
	RadioListModelQtAdapter();
	
	void set_model(std::shared_ptr<RadioListModel> new_model);

	virtual int rowCount(const QModelIndex&) const override final;
	virtual QVariant data(const QModelIndex& index, int role) const override final;

	void set_radio_names(std::vector<std::string> new_radio_names);
	void add_radio(std::string radio_name);
};
