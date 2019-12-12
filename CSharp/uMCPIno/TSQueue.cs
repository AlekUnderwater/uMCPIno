using System;
using System.Collections.Generic;
using System.Threading;

namespace uMCPIno
{
    /// <summary>
    /// Thread-safe queue
    /// </summary>
    /// <typeparam name="T">Type of items</typeparam>
    public class TSQueue<T> where T : class
    {
        #region Properties

        Queue<T> queue;

        public const int DefaultMaxQueueSize = 512;
        public const int DefaultCapacity = 512;

        public int MaxQueueSize { get; private set; }

        public int Count
        {
            get
            {
                return queue.Count;
            }
        }

        int synLock;

        #endregion

        #region Constructor

        public TSQueue()
            : this(DefaultCapacity, DefaultMaxQueueSize)
        {
        }

        public TSQueue(int capacity)
            : this(capacity, DefaultMaxQueueSize)
        {
        }

        public TSQueue(int capacity, int maxSize)
        {
            queue = new Queue<T>(capacity);
            MaxQueueSize = maxSize;
        }


        #endregion

        #region Methods

        /// <summary>
        /// Clears all items in queue, thread-safe
        /// </summary>
        public void Clear()
        {
            while (Interlocked.CompareExchange(ref synLock, 1, 0) != 0)
                Thread.SpinWait(1);

            queue.Clear();

            Interlocked.Decrement(ref synLock);
        }

        /// <summary>
        /// Enqueues item, thread-safe
        /// </summary>
        /// <param name="item">item to enqueue</param>
        public void Enqueue(T item)
        {
            while (Interlocked.CompareExchange(ref synLock, 1, 0) != 0)
                Thread.SpinWait(1);

            queue.Enqueue(item);

            if (queue.Count >= MaxQueueSize)
            {
                queue.Dequeue();
                if (QueueOverflow != null)
                    QueueOverflow(this, new EventArgs());
            }

            Interlocked.Decrement(ref synLock);

            if (ItemEnqueued != null)
            {
                ItemEnqueued.BeginInvoke(this, new EventArgs(), null, null);
            }
        }

        /// <summary>
        /// Dequeues item, thread-safe, avoid use this method on empty queue
        /// </summary>
        /// <returns>dequequed item</returns>
        public T Dequeue()
        {
            while (Interlocked.CompareExchange(ref synLock, 1, 0) != 0)
                Thread.SpinWait(1);

            T result = null;
            bool ok = false;
            if (queue.Count > 0)
            {
                result = queue.Dequeue();
                ok = true;
            }

            Interlocked.Decrement(ref synLock);

            if (ok)
                return result;
            else
                throw new InvalidOperationException("Queue empty");
        }

        /// <summary>
        /// Dequeues all items, thread-safe,  avoid use this method on empty queue
        /// </summary>
        /// <returns>array of dequeued items</returns>
        public T[] Dump()
        {
            while (Interlocked.CompareExchange(ref synLock, 1, 0) != 0)
                Thread.SpinWait(1);

            List<T> result = new List<T>();
            bool ok = false;

            if (queue.Count > 0)
            {
                result.AddRange(queue.ToArray());
                queue.Clear();
                ok = true;
            }

            Interlocked.Decrement(ref synLock);

            if (ok)
                return result.ToArray();
            else
                throw new InvalidOperationException("Queue empty");
        }

        #endregion

        #region Events

        public EventHandler ItemEnqueued;
        public EventHandler QueueOverflow;

        #endregion
    }
}
