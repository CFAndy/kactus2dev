//-----------------------------------------------------------------------------
// File: HDLParserCommon.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Janne Virtanen
// Date: 06.07.2016
//
// Description:
// The structures utilized as intermediate storage for data for HDL generation.
//-----------------------------------------------------------------------------

#ifndef HDLPARSERCOMMON_H
#define HDLPARSERCOMMON_H

#include <QSharedPointer>
#include <QString>

#include <QMap>
#include <QMultiMap>

class Component;
class ComponentInstance;
class ComponentInstantiation;
class Design;
class DesignConfiguration;
class View;
class BusInterface;
class AbstractionType;
class AbstractionDefinition;
class Port;
class Parameter;
class RemapState;

struct GenerationTuple
{
    // The component for which the generator is run. If design is not null, component
    // will refer to design or designConfiguration.
    QSharedPointer<Component> component;
    // The design, if the generator is ran for a design.
    QSharedPointer<Design> design;
    // The design configuration for design, if it is not null.
    QSharedPointer<DesignConfiguration> designConfiguration;
};

struct MetaPort;

struct MetaWire
{
    //! The bounds of the wire.
    QPair<QString,QString> bounds_;
    //! The name of the wire, to tell it apart from other wires of the same interconnection.
    QString name_;

    QList<QSharedPointer<MetaPort> > hierPorts_;
};

struct MetaPortAssignment
{
    //! The physical bounds of the assignment.
    QPair<QString,QString> physicalBounds_;
    //! The logical bounds of the assignment.
    QPair<QString,QString> logicalBounds_;
    //! The wire of interconnection where port is assigned. Is null if there is none.
    QSharedPointer<MetaWire> wire_;
    //! The assigned and parsed default value: Either a tie-off or the abstraction definition default.
    QString defaultValue_;
};

struct MetaPort
{
    //! The matching IP-XACT port.
    QSharedPointer<Port> port_;
    //! The parsed vector bounds for the port.
    QPair<QString,QString> vectorBounds_;
    //! The parsed array bounds for the port.
    QPair<QString,QString> arrayBounds_;
    //! The parsed default value for port.
    QString defaultValue_;
    
    QMultiMap<QString, QSharedPointer<MetaPortAssignment> > upAssignments_;
    QMultiMap<QString, QSharedPointer<MetaPortAssignment> > downAssignments_;
};

struct MetaInterface;

struct MetaInterconnection
{
    //! The name of the interconnection, to tell it apart from other interconnections of the design.
    QString name_;

    QMap<QString, QSharedPointer<MetaWire> > wires_;

    QList<QSharedPointer<MetaInterface> > hierIfs_;
};

struct MetaInterface
{
    //! The matching IP-XACT interface.
    QSharedPointer<BusInterface> interface_;
    //! The used abstraction type.
    QSharedPointer<AbstractionType> absType_;
    //! The abstraction definition of the abstraction type.
    QSharedPointer<AbstractionDefinition> absDef_;
    //! The used interface mode.
    QString mode_;
    //! The parsed ports of the component keyed with its physical name.
    QMap<QString, QSharedPointer<MetaPort> > ports_;
    //! The interconnection attached to the interface.
    QSharedPointer<MetaInterconnection> upInterconnection_;
    QSharedPointer<MetaInterconnection> downInterconnection_;
};

struct FormattedRemapState
{
    //! Ports and their formatted values that are to be used as the condition for using the remap.
    QList<QSharedPointer<QPair<QSharedPointer<Port>, QString> > > ports_;
    //! The matching IP-XACT remap state.
    QSharedPointer<RemapState> state_;
};

#endif // HDLPARSERCOMMON_H
