//-----------------------------------------------------------------------------
// File: ModuleParameterModel.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 29.3.2011
//
// Description:
// This model can be used to edit and update a list of module parameters.
//-----------------------------------------------------------------------------

#include "ModuleParameterModel.h"

#include "ModuleParameterColumns.h"

#include <IPXACTmodels/common/ModuleParameter.h>
#include <IPXACTmodels/common/validators/ParameterValidator.h>

#include <common/KactusColors.h>

#include <QPersistentModelIndex>

//-----------------------------------------------------------------------------
// Function: ModuleParameterModel::ModuleParameterModel()
//-----------------------------------------------------------------------------
ModuleParameterModel::ModuleParameterModel(QSharedPointer<QList<QSharedPointer<ModuleParameter> > > moduleParameters,
    QSharedPointer<QList<QSharedPointer<Choice> > > choices,
    QSharedPointer<ExpressionParser> expressionParser,
    QSharedPointer<ParameterFinder> parameterFinder,
    QSharedPointer<ExpressionFormatter> expressionFormatter,
    QObject *parent):
AbstractParameterModel(choices, 
    QSharedPointer<ParameterValidator>(new ParameterValidator(expressionParser, choices)),
    expressionParser, parameterFinder, expressionFormatter, parent),
    moduleParameters_(moduleParameters), 
    editingDisabled_(false)
{

}

//-----------------------------------------------------------------------------
// Function: ModuleParameterModel::~ModuleParameterModel()
//-----------------------------------------------------------------------------
ModuleParameterModel::~ModuleParameterModel()
{

}

//-----------------------------------------------------------------------------
// Function: ModuleParameterModel::rowCount()
//-----------------------------------------------------------------------------
int ModuleParameterModel::rowCount(QModelIndex const& parent) const 
{
	// not hierarchical model
	if (parent.isValid())
    {
		return 0;
    }

	return moduleParameters_->count();
}

//-----------------------------------------------------------------------------
// Function: ModuleParameterModel::columnCount()
//-----------------------------------------------------------------------------
int ModuleParameterModel::columnCount(QModelIndex const& parent /*= QModelIndex() */ ) const
{
	// not hierarchical model
	if (parent.isValid())
    {
		return 0;
    }

	return ModuleParameterColumns::COLUMN_COUNT;
}

//-----------------------------------------------------------------------------
// Function: ModuleParameterModel::data()
//-----------------------------------------------------------------------------
QVariant ModuleParameterModel::data(QModelIndex const& index, int role) const
{
	if (!index.isValid() || index.row() < 0 || index.row() >= rowCount())
    {
		return QVariant();
    }

    QSharedPointer<ModuleParameter> moduleParameter = getParameterOnRow(index.row()).dynamicCast<ModuleParameter>();

    if (role == Qt::DisplayRole)
    {
        if (index.column() == ModuleParameterColumns::DATA_TYPE)
        {
            return moduleParameter->getDataType();
        }
        else if (index.column() ==  ModuleParameterColumns::USAGE_TYPE)
        {
            return moduleParameter->getUsageType();
        }
    }

    else if (Qt::ForegroundRole == role)
    {
        if (!validateIndex(index))
        {
             return KactusColors::ERROR;
        }
        else if (editingDisabled_)
        {
            return KactusColors::DISABLED_TEXT;
        }
    }

    return AbstractParameterModel::data(index, role);
}

//-----------------------------------------------------------------------------
// Function: ModuleParameterModel::headerData()
//-----------------------------------------------------------------------------
QVariant ModuleParameterModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (orientation == Qt::Horizontal && role == Qt::DisplayRole) 
    {	
		if (section ==  ModuleParameterColumns::DATA_TYPE)
        {
            return tr("Data\ntype");
        }
        else if (section == ModuleParameterColumns::USAGE_TYPE)
        {
            return tr("OO usage");
        }
	}
	
    return AbstractParameterModel::headerData(section, orientation, role);
}

//-----------------------------------------------------------------------------
// Function: ModuleParameterModel::setData()
//-----------------------------------------------------------------------------
bool ModuleParameterModel::setData(QModelIndex const& index, QVariant const& value, int role)
{
	if (!index.isValid() || index.row() < 0 || index.row() >= rowCount() || editingDisabled_)
    {
		return false;
    }

	if (role == Qt::EditRole)
    {
        QSharedPointer<ModuleParameter> moduleParameter = 
            getParameterOnRow(index.row()).dynamicCast<ModuleParameter>();

        if (index.column() == ModuleParameterColumns::DATA_TYPE)
        {
            moduleParameter->setDataType(value.toString());
            emit dataChanged(index, index);
            return true;
        }
        else if (index.column() == ModuleParameterColumns::USAGE_TYPE)
        {
            moduleParameter->setUsageType(value.toString());
            emit dataChanged(index, index);
            return true;
        }
        else
        {
            return AbstractParameterModel::setData(index, value, role);
        }
	}
	else
    {
		return false;
	}
}

//-----------------------------------------------------------------------------
// Function: ModuleParameterModel::flags()
//-----------------------------------------------------------------------------
Qt::ItemFlags ModuleParameterModel::flags(QModelIndex const& index) const
{
    if (!index.isValid())
    {
        return Qt::NoItemFlags;
    }

    if (editingDisabled_)
    {
        return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
    }

    return AbstractParameterModel::flags(index);
}

//-----------------------------------------------------------------------------
// Function: ModuleParameterModel::onAddItem()
//-----------------------------------------------------------------------------
void ModuleParameterModel::onAddItem(QModelIndex const& index)
{
    if (editingDisabled_)
    {
        return;
    }

    int row = rowCount();

    // if the index is valid then add the item to the correct position
    if (index.isValid())
    {
        row = index.row();
    }

    beginInsertRows(QModelIndex(), row, row);
    moduleParameters_->insert(row, QSharedPointer<ModuleParameter>(new ModuleParameter()));
    endInsertRows();

    // tell also parent widget that contents have been changed
    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: ModuleParameterModel::onRemoveItem()
//-----------------------------------------------------------------------------
void ModuleParameterModel::onRemoveItem(QModelIndex const& index)
{
	// don't remove anything if index is invalid
	if (!index.isValid() || index.row() < 0 || index.row() >= rowCount() || editingDisabled_)
    {
		return;
	}

    if (canRemoveRow(index.row()))
    {
        // remove the specified item
        beginRemoveRows(QModelIndex(), index.row(), index.row());
        moduleParameters_->removeAt(index.row());
        endRemoveRows();
        
        // tell also parent widget that contents have been changed
        emit contentChanged();
    }
}

//-----------------------------------------------------------------------------
// Function: ModuleParameterModel::getParameterOnRow()
//-----------------------------------------------------------------------------
QSharedPointer<Parameter> ModuleParameterModel::getParameterOnRow(int row) const
{
    return moduleParameters_->at(row);
}

//-----------------------------------------------------------------------------
// Function: ModuleParameterModel::nameColumn()
//-----------------------------------------------------------------------------
int ModuleParameterModel::nameColumn() const
{
    return ModuleParameterColumns::NAME;
}

//-----------------------------------------------------------------------------
// Function: ModuleParameterModel::displayNameColumn()
//-----------------------------------------------------------------------------
int ModuleParameterModel::displayNameColumn() const
{
    return ModuleParameterColumns::DISPLAY_NAME;
}

//-----------------------------------------------------------------------------
// Function: ModuleParameterModel::formatColumn()
//-----------------------------------------------------------------------------
int ModuleParameterModel::typeColumn() const
{
    return ModuleParameterColumns::TYPE;
}

//-----------------------------------------------------------------------------
// Function: ModuleParameterModel::bitWidthLeftColumn()
//-----------------------------------------------------------------------------
int ModuleParameterModel::bitWidthLeftColumn() const
{
    return ModuleParameterColumns::BITWIDTH_LEFT;
}

//-----------------------------------------------------------------------------
// Function: ModuleParameterModel::bitWidthRightColumn()
//-----------------------------------------------------------------------------
int ModuleParameterModel::bitWidthRightColumn() const
{
    return ModuleParameterColumns::BITWIDTH_RIGHT;
}

//-----------------------------------------------------------------------------
// Function: ModuleParameterModel::minimumColumn()
//-----------------------------------------------------------------------------
int ModuleParameterModel::minimumColumn() const
{
    return ModuleParameterColumns::MINIMUM;
}

//-----------------------------------------------------------------------------
// Function: ModuleParameterModel::maximumColumn()
//-----------------------------------------------------------------------------
int ModuleParameterModel::maximumColumn() const
{
    return ModuleParameterColumns::MAXIMUM;
}

//-----------------------------------------------------------------------------
// Function: ModuleParameterModel::choiceColumn()
//-----------------------------------------------------------------------------
int ModuleParameterModel::choiceColumn() const
{
    return ModuleParameterColumns::CHOICE;
}

//-----------------------------------------------------------------------------
// Function: ModuleParameterModel::valueColumn()
//-----------------------------------------------------------------------------
int ModuleParameterModel::valueColumn() const
{
    return ModuleParameterColumns::VALUE;
}

//-----------------------------------------------------------------------------
// Function: ModuleParameterModel::resolveColumn()
//-----------------------------------------------------------------------------
int ModuleParameterModel::resolveColumn() const
{
    return ModuleParameterColumns::RESOLVE;
}

//-----------------------------------------------------------------------------
// Function: ModuleParameterModel::arrayLeftColumn()
//-----------------------------------------------------------------------------
int ModuleParameterModel::arrayLeftColumn() const
{
    return ModuleParameterColumns::ARRAY_LEFT;
}

//-----------------------------------------------------------------------------
// Function: ModuleParameterModel::arrayRightColumn()
//-----------------------------------------------------------------------------
int ModuleParameterModel::arrayRightColumn() const
{
    return ModuleParameterColumns::ARRAY_RIGHT;
}

//-----------------------------------------------------------------------------
// Function: ModuleParameterModel::descriptionColumn()
//-----------------------------------------------------------------------------
int ModuleParameterModel::descriptionColumn() const
{
    return ModuleParameterColumns::DESCRIPTION;
}

//-----------------------------------------------------------------------------
// Function: ModuleParameterModel::idColumn()
//-----------------------------------------------------------------------------
int ModuleParameterModel::idColumn() const
{
    return ModuleParameterColumns::ID;
}

//-----------------------------------------------------------------------------
// Function: ModuleParameterModel::usageCountColumn()
//-----------------------------------------------------------------------------
int ModuleParameterModel::usageCountColumn() const
{
    return ModuleParameterColumns::USAGE_COUNT;
}

//-----------------------------------------------------------------------------
// Function: ModuleParameterModel::getParameter()
//-----------------------------------------------------------------------------
QSharedPointer<ModuleParameter> ModuleParameterModel::getParameter(QModelIndex const& index) const
{
	Q_ASSERT(index.isValid());
    return  moduleParameters_->at(index.row());
}   

//-----------------------------------------------------------------------------
// Function: ModuleParameterModel::setModelParameters()
//-----------------------------------------------------------------------------
void ModuleParameterModel::setModelParameters(
    QSharedPointer<QList<QSharedPointer<ModuleParameter> > > moduleParameters)
{
    beginResetModel();
    moduleParameters_ = moduleParameters;
    endResetModel();

    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: ModuleParameterModel::enableEditing()
//-----------------------------------------------------------------------------
void ModuleParameterModel::enableEditing()
{
    editingDisabled_ = false;
}

//-----------------------------------------------------------------------------
// Function: ModuleParameterModel::disableEditing()
//-----------------------------------------------------------------------------
void ModuleParameterModel::disableEditing()
{
    editingDisabled_ = true;
}

//-----------------------------------------------------------------------------
// Function: ModuleParameterModel::validateIndex()
//-----------------------------------------------------------------------------
bool ModuleParameterModel::validateIndex(QModelIndex const& index) const
{
    QSharedPointer <Parameter> parameter = getParameterOnRow(index.row());

    /*if (index.column() == ModelParameterColumns::USAGE_TYPE)
    {
        ModelParameterValidator validator;
        return validator.hasValidUsageType(parameter.dynamicCast<ModuleParameter>().data());
    }
    else
    {*/

    return AbstractParameterModel::validateIndex(index);
//     }
}

//-----------------------------------------------------------------------------
// Function: ModuleParameterModel::indexFor()
//-----------------------------------------------------------------------------
QModelIndex ModuleParameterModel::indexFor(QSharedPointer<ModuleParameter> moduleParameter) const
{
    // find the correct row
    int row =  moduleParameters_->indexOf(moduleParameter);

    if (row < 0)
    {
        return QModelIndex();
    }

    // The base class creates the index for the row.
    return QAbstractTableModel::index(row, 0, QModelIndex());
}
