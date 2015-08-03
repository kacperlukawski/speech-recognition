#ifndef DATASOURCE_H
#define DATASOURCE_H

#include <list>
#include <memory>
#include <vector>
#include <stdexcept>
#include "DataSample.h"

using std::list;
using speech::raw_data::DataSample;

namespace speech {

    namespace raw_data {

        /**
        * This abstract class is a base for all data sources used in the application.
        * By data source we define the source of the audio signal.
        *
        */
        template<typename FrameType>
        class DataSource {
        protected:
            std::shared_ptr<list<DataSample<FrameType>>> samples;

            /** Length of the single sample in milliseconds */
            int sampleLength;

            /**
             * Returns size of single sample with length 'sizeInMilliseconds'.
             */
            virtual unsigned int getDataSampleSize(int sizeInMilliseconds) = 0;

            /**
             * Returns length of sample with size 'size'.
             */
            virtual unsigned int getDataSampleLengthInMilliseconds(int size) = 0;

        public:

            /**
             *
             */
            class offsetIterator {
            private:
                friend class DataSource;
                DataSource<FrameType> &_dataSource;
                typename list<DataSample<FrameType>>::iterator samplesIterator;
                typename list<DataSample<FrameType>>::iterator samplesIteratorEnd;
                int offset;
                int dataSampleSize;
                int offsetInMilliseconds;
                int dataSampleSizeInMilliseconds;

                std::shared_ptr<FrameType> currIterationPtr;
                int currDataSampleSize;
                int currDataSamplePosition;

                std::vector<FrameType> *windowVector;

                offsetIterator(DataSource<FrameType> &dataSource, int offsetInMilliseconds,
                         int dataSampleSizeInMilliseconds) : offsetIterator(dataSource, offsetInMilliseconds,
                                                                      dataSampleSizeInMilliseconds, false) {
                }

                offsetIterator(DataSource<FrameType> &dataSource, int offsetInMilliseconds,
                         int dataSampleSizeInMilliseconds, bool endIterator) : _dataSource(dataSource) {
                    if (offsetInMilliseconds >= dataSampleSizeInMilliseconds) {
                        throw std::invalid_argument("Sample size must be bigger than offset size.");
                    }

                    this->offsetInMilliseconds = offsetInMilliseconds;
                    this->dataSampleSizeInMilliseconds = dataSampleSizeInMilliseconds;

                    this->offset = _dataSource.getDataSampleSize(offsetInMilliseconds);
                    this->dataSampleSize = _dataSource.getDataSampleSize(dataSampleSizeInMilliseconds);

                    this->windowVector = new std::vector<FrameType>();

                    if (endIterator) {
                        this->samplesIterator = this->samplesIteratorEnd = dataSource.getSamplesIteratorEnd();
                        samplesIterator--;
                        this->currDataSamplePosition = samplesIterator->getSize();
                        this->currDataSamplePosition += 2;
                        this->currIterationPtr = samplesIterator->getValues();
                        this->currDataSampleSize = samplesIterator->getSize();
                        samplesIterator++;
                    } else {
                        this->samplesIterator = dataSource.getSamplesIteratorBegin();
                        this->samplesIteratorEnd = --dataSource.getSamplesIteratorEnd();
                        this->currIterationPtr = samplesIterator->getValues();
                        this->currDataSampleSize = samplesIterator->getSize();
                        this->currDataSamplePosition = 0;
                        ++(*this);
                    }
                }

            public:
                ~offsetIterator() {
                    windowVector->clear();
                    delete windowVector;
                    currIterationPtr.reset();
                }

                bool operator==(const offsetIterator &it) {
                    return (this->currIterationPtr.get() == it.currIterationPtr.get() &&
                            this->currDataSamplePosition == it.currDataSamplePosition);
                }

                bool operator!=(const offsetIterator &it) {
                    return !(*this == it);
                }

                // TODO: postfix operator++ is not defined

                void operator++() {
                    std::vector<FrameType> vectorTmp;
                    if (windowVector->size() > offset) {
                        for (int i = windowVector->size() - offset; i < windowVector->size(); ++i) {
                            vectorTmp.push_back((*windowVector)[i]);
                        }
                    }

                    windowVector->clear();
                    windowVector->insert(windowVector->begin(), vectorTmp.begin(), vectorTmp.end());

                    while (windowVector->size() < dataSampleSize) {
                        for (; currDataSamplePosition < currDataSampleSize &&
                               windowVector->size() < dataSampleSize; ++currDataSamplePosition) {
                            windowVector->push_back(currIterationPtr.get()[currDataSamplePosition]);
                        }
                        if (currDataSamplePosition >= currDataSampleSize) {
                            if (samplesIterator != samplesIteratorEnd) {
                                ++samplesIterator;
                                currIterationPtr = samplesIterator->getValues();
                                currDataSampleSize = samplesIterator->getSize();
                                currDataSamplePosition = 0;
                            } else {
                                ++currDataSamplePosition;
                                return;
                            }
                        }
                    }
                }

                DataSample<FrameType> operator*() {
                    int windowSize = (*windowVector).size();
                    std::shared_ptr<FrameType> windowVectorPtr(new FrameType[windowSize],
                                                               std::default_delete<FrameType[]>());
                    for (int i = 0; i < windowSize; ++i) {
                        windowVectorPtr.get()[i] = (*windowVector)[i];
                    }
                    auto size = _dataSource.getDataSampleLengthInMilliseconds(windowSize);
                    return DataSample<FrameType>(windowSize, size, windowVectorPtr);
                }
            };

            DataSource(int sampleLength);

            virtual ~DataSource();

            virtual void addSample(DataSample<FrameType> sample);

            virtual typename list<DataSample<FrameType>>::iterator getSamplesIteratorBegin();

            virtual typename list<DataSample<FrameType>>::iterator getSamplesIteratorEnd();

            virtual DataSource<FrameType>::offsetIterator getOffsetIteratorBegin(int windowSizeInMilliseconds,
                                                                           int offsetInMilliseconds);

            virtual DataSource<FrameType>::offsetIterator getOffsetIteratorEnd();
        };

    }

}

#endif