//-----------------------------------------------------------------------------
// File: PortVerilogWriter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 28.7.2014
//
// Description:
// Class for writing port declarations in Verilog.
//-----------------------------------------------------------------------------

#ifndef PORTVERILOGWRITER_H
#define PORTVERILOGWRITER_H

#include "../veriloggeneratorplugin_global.h"
#include "../common/Writer.h"

#include <Plugins/common/HDLParser/HDLParserCommon.h>

#include <QTextStream>

//-----------------------------------------------------------------------------
// Class for writing port declarations in Verilog.
//-----------------------------------------------------------------------------
class VERILOGGENERATORPLUGIN_EXPORT PortVerilogWriter : public Writer
{
public:
    /*!
     *  The constructor.
     *
     *      @param [in] port                    The port to write to Verilog.
     */
    PortVerilogWriter(QSharedPointer<MetaPort> port);

    //! The destructor.
    ~PortVerilogWriter();

	/*! Writes the Verilog port declaration into a text stream.
	 *
	 *      @param [in] output   The text stream to write the port into.
	 */
    void write(QTextStream& outputStream) const;

private:
    
    //! No copying.
    PortVerilogWriter(const PortVerilogWriter& other);

    //! No assignment.
    PortVerilogWriter& operator=(const PortVerilogWriter& other);

    /*!
     *  Checks if the writer should write nothing.
     *
     *      @return True, if the writer has nothing to write, otherwise false.
     */
    bool nothingToWrite() const;

    /*!
     *  Creates a Verilog port declaration from the port model.     
     *
     *      @return A Verilog port declaration.
     */
    QString createDeclaration() const; 

    /*!
     *  Creates a string representation of the port direction.
     *
     *      @return The port direction.
     */
    QString direction() const;
    
    /*!
     *  Returns vector bounds representation for the port.
     */
    QString formatBounds(QPair<QString, QString> const& bounds) const;
    
    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The port to write to Verilog.
    QSharedPointer<const MetaPort> port_;
};

#endif // PORTVERILOGWRITER_H
