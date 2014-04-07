#include "radio_list_model_qt_adapter.hpp"

RadioListModelQtAdapter::RadioListModelQtAdapter()
{
}

void RadioListModelQtAdapter::set_model(std::shared_ptr<RadioListModel> new_model)
{
	model = new_model;
}

int RadioListModelQtAdapter::rowCount(const QModelIndex&) const
{
	return model->row_count();
}

QVariant RadioListModelQtAdapter::data(const QModelIndex& index, int role) const
{
	if(role != Qt::DisplayRole) {
		return QVariant();
	}

	if(!index.isValid() && index.row() >= 0 && index.row() < rowCount(index)) {
		return QVariant();
	}

	return QVariant(QString::fromStdString(model->get_data(index.row())));
}

void RadioListModelQtAdapter::set_radio_names(std::vector<std::string> new_radio_names)
{
	beginResetModel();
	model->set_radio_names(new_radio_names);
	endResetModel();
}

void RadioListModelQtAdapter::add_radio(std::string radio_name)
{
	beginInsertRows(QModelIndex(), rowCount(QModelIndex()), rowCount(QModelIndex()));
	model->add_radio(radio_name);
	endInsertRows();
}
