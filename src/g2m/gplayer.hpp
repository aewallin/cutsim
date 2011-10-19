
#ifndef GPLAYER_HH
#define GPLAYER_HH

#include <vector>
#include <limits.h>
#include <iostream>
#include <fstream>

#include <QString>
#include <QProcess>
#include <QObject>
#include <QtDebug>

#include "canonLine.hpp"
#include "nanotimer.hpp"

namespace g2m {

/**
\class GPlayer
\brief This class loops through the list of canonLine objects produced by g2m.
*/
class GPlayer : public QObject {
    Q_OBJECT;
    public:
        GPlayer()  {  
            first = true;
        }
    public slots:
        /// start or resume executing the program
        void play() {
            qDebug() << " gplayer::play() ";
            slotRequestMove();
        }
        
        /*
         * {
            nanotimer timer;
            timer.start();
            emit debugMessage( tr("GPlayer: play") );
            int tool;
            bool first=true;
            for( unsigned int n=0;n<lines.size();n++ ) {
                canonLine* cl = lines[n];
                if (first) {
                    tool = cl->getStatus()->getTool();
                    first = false;
                }
                if (cl->isMotion() ) {
                    Point start = cl->getStart().loc;
                    Point end = cl->getEnd().loc;
                    // divide motion into sampled points. signal motion along path.
                    double ds = 0.5;
                    int n_samples = std::max( (int)( cl->length()/ds ) , 2 ); // want at least two points: start-end
                    //emit debugMessage( tr("GPlayer: length= %1 so will sample %2 moves for this move").arg(cl->length()).arg( n_samples ) );
                    double move_length = cl->length();
                    emit debugMessage( tr("GPlayer: %1").arg( cl->getLineNum() ) + 
                                       tr(" motion from (%1, %2, %3)").arg(start.x).arg(start.y).arg(start.z) +
                                       tr(" to (%1, %2, %3)").arg(end.x).arg(end.y).arg(end.z) +
                                       tr(" length = %1").arg( cl->length()) +
                                       tr(" n_samples = %1").arg( n_samples) 
                                       );
                    for (int m=0; m<n_samples;m++) {
                        // FIXME: handle first and last moves differently?
                        Point pos = cl->point( (double)(m)/(double)(n_samples-1)*move_length );
                        //emit debugMessage( tr("GPlayer: move: (%1, %2, %3)").arg(pos.x).arg(pos.y).arg(pos.z) );
                        emit signalToolPosition( pos.x, pos.y, pos.z );
                    }
                } else {
                    // not motion, check for toolchange
                    if ( tool != cl->getStatus()->getTool() ) {
                        emit debugMessage( tr("GPlayer: toolchange to %1").arg( cl->getStatus()->getTool() ) );
                        emit signalToolChange( cl->getStatus()->getTool() );
                        tool = cl->getStatus()->getTool();
                    }
                }
                emit signalProgress( (int)(100*n/(lines.size()-1)) ); // report progress to ui
            }
            double e = timer.getElapsedS();
            emit debugMessage( tr("Gplayer: play() took ") + timer.humanreadable(e)  ) ;
        }*/
        /// signal the next move
        void slotRequestMove() {
            // UI request that we signal the next signalToolPosition()
            canonLine* cl = lines[current_line];
            if (first) {// first ever call here
                //current_line = 0;
                current_tool = cl->getStatus()->getTool();
                first = false;
            }   
            if (cl->isMotion() ) {
                //Point start = cl->getStart().loc;
                //Point end = cl->getEnd().loc;
                // divide motion into sampled points. signal motion along path.
                double ds = 0.5;
                int n_samples = std::max( (int)( cl->length()/ds ) , 2 ); // want at least two points: start-end
                //emit debugMessage( tr("GPlayer: length= %1 so will sample %2 moves for this move").arg(cl->length()).arg( n_samples ) );
                double move_length = cl->length();
                //for (int m=0; m<n_samples;m++) {
                    // FIXME: handle first and last moves differently?
                Point pos = cl->point( (double)(m)/(double)(n_samples-1)*move_length );
                if (m == (n_samples-1) )
                    move_done = true;
                //emit debugMessage( tr("GPlayer: move: (%1, %2, %3)").arg(pos.x).arg(pos.y).arg(pos.z) );
                emit signalToolPosition( pos.x, pos.y, pos.z );
                m++; // advance along the move
            } else {
                // not motion, check for toolchange
                if ( current_tool != cl->getStatus()->getTool() ) {
                    emit debugMessage( tr("GPlayer: toolchange to %1").arg( cl->getStatus()->getTool() ) );
                    emit signalToolChange( cl->getStatus()->getTool() );
                    current_tool = cl->getStatus()->getTool();
                }
                slotRequestMove(); // call self!
            }
            
            if (move_done) {
                current_line++;
                move_done=false;
                m=0;
            }
            emit signalProgress( (int)(100*current_line/(lines.size()-1)) ); // report progress to ui
        }
        /// pause program
        void pause() {
            emit debugMessage( tr("GPlayer: pause") );
        }
        /// stop the execution of the g-code program
        void stop() {
            emit debugMessage( tr("GPlayer: stop") );
        }
        /// add a canonLine to the vector of lines to be processed
        /// \param l the new canonLine to be added
        void appendCanonLine( canonLine* l) {
            lines.push_back(l);
        }
    signals:
        /// signal a new tool position
        void signalToolPosition( double x, double y, double z ); // three-axis for now..
        /// signal a tool change to new tool \param t
        void signalToolChange( int t );
        /// signal the UI how far along the g-code program we are
        void signalProgress( int p ); 
        /// signal a debug message
        void debugMessage(QString s);
    protected:
        /// flag for first move of g-code
        bool first;
        /// index of current tool
        int current_tool;
        /// the current canonLine being processed
        unsigned int current_line;
        /// loop variable
        int m;
        /// flag indicating when current move done
        bool move_done;
        /// vector of canonLines to process
        std::vector<canonLine*> lines;
};
              /*  
                emit debugMessage( tr("GPlayer: %1").arg( cl->getLineNum() ) + 
                                   tr(" motion from (%1, %2, %3)").arg(start.x).arg(start.y).arg(start.z) +
                                   tr(" to (%1, %2, %3)").arg(end.x).arg(end.y).arg(end.z) +
                                   tr(" length = %1").arg( cl->length()) +
                                   tr(" n_samples = %1").arg( n_samples) 
                                   );
*/
} // end namespace
#endif // GPlayer_HH
