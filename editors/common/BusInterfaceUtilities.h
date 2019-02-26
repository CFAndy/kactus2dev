//-----------------------------------------------------------------------------
// File: BusInterfaceUtilities.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 15.02.2019
//
// Description:
// Utilities for bus interface mode compatibilities.
//-----------------------------------------------------------------------------

#ifndef BUSINTERFACECOMPATIBILITIES_H
#define BUSINTERFACECOMPATIBILITIES_H

#include <QSharedPointer>

class BusDefinition;
class VLNV;
class LibraryInterface;

//-----------------------------------------------------------------------------
//! Utilities for bus interface mode compatibilities.
//-----------------------------------------------------------------------------
namespace BusInterfaceUtilities
{

    /*!
     *  Checks if the selected bus definitions match.
     *
     *      @param [in] firstDefinition         The first bus definition.
     *      @param [in] secondDefinitionVLNV    VLNV of the second bus definition.
     *
     *      @return True, if the bus definitions match, false otherwise.
     */
    bool hasMatchingBusDefinitions(QSharedPointer<const BusDefinition> firstDefinition,
        VLNV const& secondDefinitionVLNV, LibraryInterface* library);
}

//-----------------------------------------------------------------------------

#endif // BUSINTERFACECOMPATIBILITIES_H