//-----------------------------------------------------------------------------
// File: MemoryConnectionItem.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 16.08.2016
//
// Description:
// Memory designer connection item between address spaces and memory maps.
//-----------------------------------------------------------------------------

#ifndef MEMORYCONNETIONITEM_H
#define MEMORYCONNETIONITEM_H

class MainMemoryGraphicsItem;

#include <QGraphicsPathItem>
#include <QGraphicsTextItem>
#include <QGraphicsScene>

//-----------------------------------------------------------------------------
//! Memory designer connection item between address spaces and memory maps.
//-----------------------------------------------------------------------------
class MemoryConnectionItem : public QGraphicsPathItem
{

public:

    /*!
     *  The constructor.
     *
     *      @param [in] startItem                   The start item of the connection (address space).
     *      @param [in] firstStartValue             Start address in the start item.
     *      @param [in] firstEndValue               End address in the start item.
     *      @param [in] endItem                     The end item of the connection (memory map).
     *      @param [in] containingScene             Graphics scene containing the memory connection item.
     *      @param [in] memoryItemsAreCondensed     Value for displaying condensed memory connections.
     *      @param [in] yTransfer                   Y transfer of the memory connection.
     *      @param [in] parent                      Parent item of the connection.
     */
    MemoryConnectionItem(MainMemoryGraphicsItem* startItem, quint64 firstStartValue, quint64 firstEndValue,
        MainMemoryGraphicsItem* endItem, QGraphicsScene* containingScene, bool memoryItemsAreCondensed,
        int yTransfer = 0, QGraphicsItem* parent = 0);

	/*!
     *  The Destructor.
     */
    virtual ~MemoryConnectionItem();

    /*!
     *  Move a memory connection item by the y-coordinate.
     *
     *      @param [in] movementOrigin  The origin of the movement.
     *      @param [in] yTransfer       The movement amount.
     */
    void onMoveConnectionInY(MainMemoryGraphicsItem* movementOrigin, qreal yTransfer);

    /*!
     *  Get the start value of the connection.
     *
     *      @return The start value of the connection.
     */
    quint64 getRangeStartValue() const;

    /*!
     *  Get the end value of the connection.
     *
     *      @return The end value of the connection.
     */
    quint64 getRangeEndValue() const;

    /*!
     *  Redraw the path of this connection.
     */
    void reDrawConnection();

    /*!
     *  Move the other connected item.
     *
     *      @param [in] movementOrigin  The origin item of the movement.
     *      @param [in] yTransfer       The movement amount.
     */
    void moveConnectedItem(MainMemoryGraphicsItem* movementOrigin, qreal yTransfer);

    /*!
     *  Get the lowest point of the connection.
     *
     *      @return The lowest point of the connection item.
     */
    quint64 getSceneEndPoint() const;

    /*!
     *  Get the connected start memory item.
     *
     *      @return The connected start memory item.
     */
    MainMemoryGraphicsItem* getConnectionStartItem() const;

    /*!
     *  Get the connected end memory item.
     *
     *      @return The connected end memory item.
     */
    MainMemoryGraphicsItem* getConnectionEndItem() const;

    /*!
     *  Reposition range labels that are colliding with range labels of other memory connection items.
     */
    void repositionCollidingRangeLabels();

    /*!
     *  Check if the selected label collides with the range labels.
     *
     *      @param [in] label           The selected label.
     *      @param [in] fontHeight      Height of the font used in the selected label.
     *      @param [in] connectedItem   Owner of the selected label.
     *
     *      @return True, if the selected label collides with the range labels, false otherwise.
     */
    bool labelCollidesWithRanges(QGraphicsTextItem* label, qreal fontHeight,
        const MainMemoryGraphicsItem* connectedItem) const;

    /*!
     *  Condense this connection to contain the selected addresses.
     *
     *      @param [in] uncutAddresses  The addresses that remain after the compression.
     *      @param [in] CUTMODIFIER     Modifier for the cut areas.
     */
    void condenseToUnCutAddresses(QVector<quint64> uncutAddresses, const int CUTMODIFIER);

private:
    // Disable copying.
    MemoryConnectionItem(MemoryConnectionItem const& rhs);
    MemoryConnectionItem& operator=(MemoryConnectionItem const& rhs);

    /*!
     *  Compress the contained end item.
     *
     *      @param [in] unCutAddresses  The addresses that remain after the compression.
     *      @param [in] CUTMODIFIER     Modifier for the cut areas.
     */
    void compressEndItem(QVector<quint64> unCutAddresses, const int CUTMODIFIER);

    /*!
     *  Set a new height for the memory connection.
     *
     *      @param [in] newHeight   The new height.
     */
    void setCondensedHeight(qreal newHeight);

    /*!
     *  Setup the range labels.
     *
     *      @param [in] startValue  Base address of the connection.
     *      @param [in] endValue    Last address of the connection.
     */
    void setupLabels(quint64 startValue, quint64 endValue);

    /*!
     *  Create the connection path.
     */
    void createPath();

    /*!
     *  Avoid collisions on the connection path.
     *
     *      @param [in] highStartPoint  The top start point of the connection.
     *      @param [in] highEndPoint    The top end point of the connection.
     *      @param [in] lowStartPoint   The low start point of the connection.
     *      @param [in] lowEndPoint     The low end point of the connection.
     */
    void avoidCollisionsOnPath(QPointF highStartPoint, QPointF highEndPoint, QPointF lowStartPoint, QPointF lowEndPoint);

    /*!
     *  Create a collision path according to the collision points.
     *
     *      @param [in] highStartPoint          The top start point of the connection.
     *      @param [in] highEndPoint            The top end point of the connection.
     *      @param [in] lowStartPoint           The low start point of the connection.
     *      @param [in] lowEndPoint             The low end point of the connection.
     *      @param [in] highCollisionPoints     Map containing the top collision points of the connection item.
     *      @param [in] lowCollisionPoints      Map containing the low collision points of the connection item.
     */
    void createCollisionPath(QPointF highStartPoint, QPointF highEndPoint, QPointF lowStartPoint,
        QPointF lowEndPoint, QMap<qreal, QPair<QPointF, QPointF> > highCollisionPoints,
        QMap<qreal, QPair<QPointF, QPointF> > lowCollisionPoints);

    /*!
     *  Reposition the connection labels.
     */
    void repositionLabels();

    /*!
     *  Get the width of the connection.
     *
     *      @return The width of the connection.
     */
    qreal getConnectionWidth() const;

    /*!
     *  Reposition a single colliding range label.
     *
     *      @param [in] textLabel   The selected range label.
     */
    void repositionSingleRangeLabel(QGraphicsTextItem* rangeLabel) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The label containing the first item start range.
    QGraphicsTextItem* firstItemStartLabel_;

    //! The label containing the first item end range.
    QGraphicsTextItem* firstItemEndLabel_;

    //! The label containing the second item start range.
    QGraphicsTextItem* secondItemStartLabel_;

    //! The label containing the second item end range.
    QGraphicsTextItem* secondItemEndLabel_;

    //! The connection start memory graphics item.
    MainMemoryGraphicsItem* startItem_;

    //! The connection end memory graphics item.
    MainMemoryGraphicsItem* endItem_;

    //! Connection start range.
    quint64 connectionBaseAddress_;

    //! Connection end range.
    quint64 connectionLastAddress_;

    //! Y transfer of the connection.
    int yTransfer_;

    //! Width of the memory connection item.
    qreal connectionWidth_;

    //! Value for displaying condensed memory connections.
    bool memoryItemsAreCondensed_;
};

//-----------------------------------------------------------------------------


#endif // MEMORYCONNETIONITEM_H
