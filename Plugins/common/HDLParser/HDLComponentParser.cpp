//-----------------------------------------------------------------------------
// File: HDLComponentParser.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Janne Virtanen
// Date: 11.08.2016
//
// Description:
// Class used to parse relevant information from IP-XACT component for HDL generation.
//-----------------------------------------------------------------------------

#include "HDLComponentParser.h"

#include <library/LibraryManager/libraryinterface.h>

#include <IPXACTmodels/AbstractionDefinition/AbstractionDefinition.h>

#include <IPXACTmodels/common/ModuleParameter.h>
#include <IPXACTmodels/common/VLNV.h>
#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/BusInterface.h>
#include <IPXACTmodels/Component/Port.h>
#include <IPXACTmodels/Component/View.h>
#include <IPXACTmodels/Component/ComponentInstantiation.h>

#include <IPXACTmodels/Component/MemoryMap.h>
#include <IPXACTmodels/Component/MemoryRemap.h>
#include <IPXACTmodels/Component/MemoryBlockBase.h>
#include <IPXACTmodels/Component/AddressBlock.h>
#include <IPXACTmodels/Component/Register.h>
#include <IPXACTmodels/Component/Field.h>
#include <IPXACTmodels/Component/RemapState.h>
#include <IPXACTmodels/Component/RemapPort.h>

#include "editors/ComponentEditor/common/ComponentParameterFinder.h"
#include "editors/ComponentEditor/common/ExpressionFormatter.h"

//-----------------------------------------------------------------------------
// Function: HDLComponentParser::HDLComponentParser
//-----------------------------------------------------------------------------
HDLComponentParser::HDLComponentParser(LibraryInterface* library, QSharedPointer<Component> component) :
library_(library),
retval_(new GenerationComponent)
{
    retval_->component = component;
}

//-----------------------------------------------------------------------------
// Function: HDLComponentParser::~HDLComponentParser
//-----------------------------------------------------------------------------
HDLComponentParser::~HDLComponentParser()
{
}

//-----------------------------------------------------------------------------
// Function: HDLComponentParser::parseComponent
//-----------------------------------------------------------------------------
void HDLComponentParser::parseComponent(QSharedPointer<View> activeView)
{
    activeView_ = activeView;

    if (!activeView_)
    {
        return;
    }

    QSharedPointer<ComponentInstantiation> currentInsta =
        retval_->component->getModel()->findComponentInstantiation(activeView_->getComponentInstantiationRef());

    if (currentInsta && !currentInsta->getModuleName().isEmpty())
    {
        retval_->moduleName_ = currentInsta->getModuleName();
    }
    else
    {
        retval_->moduleName_ = retval_->component->getVlnv().getName();
    }

    QSharedPointer<ComponentParameterFinder> parameterFinder(new ComponentParameterFinder(retval_->component));
    parameterFinder->setActiveView(activeView);
    formatter_ = QSharedPointer<ExpressionFormatter>(new ExpressionFormatter(parameterFinder));

    parseInterfaces(retval_);
    parsePorts(retval_);
    parseMemoryMaps(retval_);
    findParameters(retval_);
    parseRemapStates(retval_);
}

//-----------------------------------------------------------------------------
// Function: HDLComponentParser::sortModuleParameters()
//-----------------------------------------------------------------------------
void HDLComponentParser::sortParameters(QList<QSharedPointer<Parameter> >& refParameters,
    QList<QSharedPointer<Parameter> >& sortParameters )
{
    // Go through existing ones on the instance.
    for (QList<QSharedPointer<Parameter> >::Iterator parameterAdd =
        refParameters.begin(); parameterAdd != refParameters.end(); ++parameterAdd)
    {
        // The first position for the second pass.
        QList<QSharedPointer<Parameter> >::Iterator minPos =
            sortParameters.begin();

        // The value of the inspected parameter.
        QString valueAdd = (*parameterAdd)->getValue();

        // First pass: Detect if the parameter depends on another parameter.
        for (QList<QSharedPointer<Parameter> >::Iterator parameterCmp =
            sortParameters.begin();
            parameterCmp != sortParameters.end(); ++parameterCmp)
        {
            if (valueAdd.contains((*parameterCmp)->getValueId()))
            {
                // A match found: The parameter must be positioned after this one!
                minPos = ++parameterCmp;

                // The first one needed is the relevant one, so break here.
                break;
            }
        }

        // If true, the parameter will be appended to the end of the list.
        bool append = true;

        // The second pass: Find the actual position before the parameter is referred.
        for (QList<QSharedPointer<Parameter> >::Iterator parameterCmp = minPos;
            parameterCmp != sortParameters.end(); ++parameterCmp)
        {
            // The value of the the compared parameter.
            QString valueCmp = (*parameterCmp)->getValue();

            // Check if it contains a reference to the inspected parameter.
            if (valueCmp.contains((*parameterAdd)->getValueId()))
            {
                // Take a direct shared pointer to the referring parameter.
                QSharedPointer<Parameter> parameterRef = *parameterCmp;

                // Remove the inspected parameter from the previous place.
                sortParameters.removeOne(*parameterAdd);

                // See where the referring parameter is located now.
                int cmpLoc = sortParameters.indexOf(*parameterCmp);

                // Then the inspected parameter comes before it is referred.
                sortParameters.insert(cmpLoc, *parameterAdd);

                // It will not be inserted twice, so break here.
                append = false;
                break;
            }
        }

        // If there was no match in the second pass, or no second pass at all, append to the end.
        if (append)
        {
            // Remove the inspected parameter from the previous place.
            sortParameters.removeOne(*parameterAdd);
            // Append at the end of the list.
            sortParameters.append(*parameterAdd);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: HDLComponentParser::parseInterfaces()
//-----------------------------------------------------------------------------
void HDLComponentParser::parseInterfaces(QSharedPointer<GenerationComponent> retval)
{
    foreach(QSharedPointer<BusInterface> busInterface, *retval_->component->getBusInterfaces())
    {
        QSharedPointer<GenerationInterface> gif(new GenerationInterface);
        gif->description = busInterface->description();
        gif->mode = interfaceMode2Str(busInterface->getInterfaceMode());
        gif->name = busInterface->name();
        gif->interface = busInterface;

        ConfigurableVLNVReference busRef = busInterface->getBusType();
        gif->typeName = busRef.getName();

        if (busInterface->getAbstractionTypes()->count() > 0)
        {
            QSharedPointer<ConfigurableVLNVReference> absRef = busInterface->getAbstractionTypes()->first()->getAbstractionRef();
            QSharedPointer<AbstractionDefinition> absDef;

            if (absRef)
            {
                QSharedPointer<Document> docAbsDef = library_->getModel(*absRef);

                if (docAbsDef)
                {
                    absDef = docAbsDef.dynamicCast<AbstractionDefinition>();

                    if (absDef)
                    {
                        gif->fileName =  absDef->getFileName();
                    }
                }
            }
        }

        retval->interfaces.insert(busInterface->name(), gif);
    }
}

//-----------------------------------------------------------------------------
// Function: HDLComponentParser::parsePorts()
//-----------------------------------------------------------------------------
void HDLComponentParser::parsePorts(QSharedPointer<GenerationComponent> retval)
{
    foreach (QSharedPointer<Port> cport, *retval_->component->getPorts())
    {
        QSharedPointer<GenerationPort> gport(new GenerationPort);

        QSharedPointer<QList<QSharedPointer<BusInterface> > > busInterfaces =
            retval_->component->getInterfacesUsedByPort(cport->name());

        // Cull all interfaces that utilize the port.
        foreach (QSharedPointer<BusInterface> busInterface, *busInterfaces)
        {
            QSharedPointer<GenerationInterface> gif = retval->interfaces[busInterface->name()];

            if (gif)
            {
                gport->interfaces.append(gif);
            }
        }

        // Needs a reference to the IP-XACT port.
        gport->port = cport;

        // Both vector and array bounds may be needed.
        gport->arrayBounds.first = formatter_->formatReferringExpression(cport->getArrayLeft());
        gport->arrayBounds.second = formatter_->formatReferringExpression(cport->getArrayRight());

        gport->vectorBounds.first = formatter_->formatReferringExpression(cport->getLeftBound());
        gport->vectorBounds.second = formatter_->formatReferringExpression(cport->getRightBound());

        // Add to the list.
        retval->ports.insert(cport->name(),gport);
    }
}

//-----------------------------------------------------------------------------
// Function: HDLComponentParser::parseMemoryMaps
//-----------------------------------------------------------------------------
void HDLComponentParser::parseMemoryMaps(QSharedPointer<GenerationComponent> target) const
{
    // TODO: replace with a foreach structure
    if (retval_->component->getMemoryMaps()->size() < 1)
    {
        return;
    }

	QSharedPointer<MemoryMap> memmap = retval_->component->getMemoryMaps()->first();

	// The AUB is shared between all remap states.
	target->aub = memmap->getAddressUnitBits();

    QSharedPointer<GenerationRemap> defaultRemap(new GenerationRemap);
    target->remaps.append(defaultRemap);
    defaultRemap->name = memmap->name() + "_default";

	foreach (QSharedPointer<MemoryBlockBase> mbb, *memmap->getMemoryBlocks())
	{
		QSharedPointer<AddressBlock> ab = mbb.dynamicCast<AddressBlock>();
		
		// Skip if does not convert to correct type.
		if (!ab)
		{
			continue;
		}

        // Get the total sum of ranges.
		target->totalRange += ab->getRange();

        parseAddressBlock(ab, defaultRemap);
	}

    // The default map defines the size. We assume that any remaps respect there boundaries.
    target->totalRange = formatter_->formatReferringExpression(target->totalRange);

	foreach (QSharedPointer<MemoryRemap> remap, *memmap->getMemoryRemaps())
    {
        QSharedPointer<GenerationRemap> rm(new GenerationRemap);
        rm->name = remap->name();
        target->remaps.append(rm);

		foreach (QSharedPointer<MemoryBlockBase> mbb, *remap->getMemoryBlocks())
	    {
			QSharedPointer<AddressBlock> ab = mbb.dynamicCast<AddressBlock>();

			// Skip if does not convert to correct type.
			if (!ab)
			{
				continue;
			}

            parseAddressBlock(ab, rm);
		}
	}
}

//-----------------------------------------------------------------------------
// Function: HDLComponentParser::parseAddressBlock
//-----------------------------------------------------------------------------
void HDLComponentParser::parseAddressBlock(QSharedPointer<AddressBlock> ab, QSharedPointer<GenerationRemap> target) const
{
    // Create a new address block for generation.
    QSharedPointer<GenerationAddressBlock> gab(new GenerationAddressBlock);

    // Its base address and name are needed.
    gab->baseAddress_ = formatter_->formatReferringExpression(ab->getBaseAddress());
    gab->name = ab->name();

    // Append to the list.
    target->blocks.append(gab);

    foreach (QSharedPointer<RegisterBase> rb, *ab->getRegisterData())
    {
        QSharedPointer<Register> r = rb.dynamicCast<Register>();

        // Skip if does not convert to correct type.
        if (!r)
        {
            continue;
        }

        QSharedPointer<GenerationRegister> gr(new GenerationRegister);

        if (r->getDimension().isEmpty())
        {
            // No dimension is interpreted as one.
            gr->dimension_ = "1";
        }
        else
        {
            // Else take it via formatting.
            gr->dimension_ = formatter_->formatReferringExpression(r->getDimension());
        }

        // Pick also other needed values via formatter, except the name.
        gr->offset_ = formatter_->formatReferringExpression(r->getAddressOffset());
        gr->size_ = formatter_->formatReferringExpression(r->getSize());
        gr->name = r->name();

        // Append to the list.
        gab->registers.append(gr);

        foreach (QSharedPointer<Field> f, *r->getFields())
        {
            QSharedPointer<GenerationField> gf(new GenerationField);

            // Pick also the values needed by a field via formatter, except the name.
            gf->bitOffset_ = formatter_->formatReferringExpression(f->getBitOffset());
            gf->bitWidth_ = formatter_->formatReferringExpression(f->getBitWidth());
            gf->name = f->name();

            // Append to the list.
            gr->fields.append(gf);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: HDLComponentParser::dindParameters()
//-----------------------------------------------------------------------------
void HDLComponentParser::findParameters(QSharedPointer<GenerationComponent> target) const
{
    QSharedPointer<ComponentInstantiation> currentInsta =
        retval_->component->getModel()->findComponentInstantiation(activeView_->getComponentInstantiationRef());

    foreach(QSharedPointer<Parameter> parameter, *retval_->component->getParameters())
    {
        target->originalParameters.append(parameter);
    }

	if (currentInsta)
	{
		foreach(QSharedPointer<ModuleParameter> parameter, *currentInsta->getModuleParameters())
		{
			target->originalParameters.append(parameter);
		}
    }

    // Create a new list of the parameters.
    QList<QSharedPointer<Parameter> > sortedParameters = QList<QSharedPointer<Parameter> >(target->originalParameters);

	// Sort the parameters.
    sortParameters(target->originalParameters, sortedParameters);

    foreach(QSharedPointer<Parameter> parameterOrig, sortedParameters)
    {
        QSharedPointer<Parameter> parameterCpy(new Parameter(*parameterOrig));

        // Format the parameter.
        parameterCpy->setValue(formatter_->formatReferringExpression(parameterOrig->getValue()));

        target->formattedParameters.append(parameterCpy);
    }
}

//-----------------------------------------------------------------------------
// Function: HDLComponentParser::parseRemapStates
//-----------------------------------------------------------------------------
void HDLComponentParser::parseRemapStates(QSharedPointer<GenerationComponent> target) const
{
    // TODO: replace with a foreach structure
    if (retval_->component->getMemoryMaps()->size() < 1)
    {
        return;
    }

    QSharedPointer<MemoryMap> memmap = retval_->component->getMemoryMaps()->first();

    // Each remap is may have a remap state.
    foreach (QSharedPointer<MemoryRemap> remap, *memmap->getMemoryRemaps())
    {
        QString stateName = remap->getRemapState();

        QSharedPointer<RemapState> state;
        
        // Try to match the remap state reference to a remap state within the component.
        foreach(QSharedPointer<RemapState> currentState, *retval_->component->getRemapStates())
        {
            if (currentState->name() == stateName)
            {
                state = currentState;
                break;
            }
        }

        // If there is none, skip.
        if (!state)
        {
            continue;
        }

        // Create new remap state for the generation.
        QSharedPointer<GenerationRemapState> grms(new GenerationRemapState);
        grms->stateName = stateName;
        target->remapStates.append(grms);

        // Each port referred by the state must be listed.
        foreach(QSharedPointer<RemapPort> rmport, *state->getRemapPorts())
        {
           QSharedPointer<QPair<QSharedPointer<Port>,QString> > parsedPort(new QPair<QSharedPointer<Port>,QString>);
           
           // Pick the port name, and the value needed for it to remap state become effective.
           parsedPort->first = retval_->component->getPort(rmport->getPortNameRef());
           parsedPort->second = formatter_->formatReferringExpression(rmport->getValue());

           grms->ports.append(parsedPort);
        }
    }
}