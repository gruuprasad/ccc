struct course
{
  int marks;
  char subject[30];
};

int main()
{
  struct course *ptr;
  int i, noOfRecords;
  printf("Enter number of records: ");
  scanf("%d", &noOfRecords);

  // Allocates the memory for noOfRecords structures with pointer ptr pointing to the base address.
  ptr = (struct course*) malloc (noOfRecords * sizeof(struct course));

  for(i = 0; i < noOfRecords; ++i)
  {
    printf("Enter name of the subject and marks respectively:\n");
    scanf("%s %d", &(ptr+i)->subject, &(ptr+i)->marks);
  }

  printf("Displaying Information:\n");

  for(i = 0; i < noOfRecords ; ++i)
    printf("%s\t%d\n", (ptr+i)->subject, (ptr+i)->marks);

  return 0;
}

int main()
{
  int i, j, rows;

  printf("Enter number of rows: ");
  scanf("%d",&rows);

  for(i=1; i<=rows; ++i)
  {
    for(j=1; j<=i; ++j)
    {
      printf("* ");
    }
    printf("\n");
  }
  return 0;
}

int main()
{
  int i, j, rows;

  printf("Enter number of rows: ");
  scanf("%d",&rows);

  for(i=1; i<=rows; ++i)
  {
    for(j=1; j<=i; ++j)
    {
      printf("%d ",j);
    }
    printf("\n");
  }
  return 0;
}

int main()
{
  int i, space, rows, k=0;

  printf("Enter number of rows: ");
  scanf("%d",&rows);

  for(i=1; i<=rows; ++i, k=0)
  {
    for(space=1; space<=rows-i; ++space)
    {
      printf("  ");
    }

    while(k != 2*i-1)
    {
      printf("* ");
      ++k;
    }

    printf("\n");
  }

  return 0;
}

int main()
{
  int i, space, rows, k=0, count = 0, count1 = 0;

  printf("Enter number of rows: ");
  scanf("%d",&rows);

  for(i=1; i<=rows; ++i)
  {
    for(space=1; space <= rows-i; ++space)
    {
      printf("  ");
      ++count;
    }

    while(k != 2*i-1)
    {
      if (count <= rows-1)
      {
        printf("%d ", i+k);
        ++count;
      }
      else
      {
        ++count1;
        printf("%d ", (i+k-2*count1));
      }
      ++k;
    }
    count1 = count = k = 0;

    printf("\n");
  }
  return 0;
}

int main() {
  FILE *fp;
  int c;
  fp = fopen(__FILE__,"r");
  do {
    c = getc(fp);
    putchar(c);
  }
  while(c != EOF);
  fclose(fp);
  return 0;
}

int main()
{
  int low, high, i, flag;
  printf("Enter two numbers(intervals): ");
  scanf("%d %d", &low, &high);

  printf("Prime numbers between %d and %d are: ", low, high);

  while (low < high)
  {
    flag = 0;

    for(i = 2; i <= low/2; ++i)
    {
      if(low % i == 0)
      {
        flag = 1;
        break;
      }
    }

    if (flag == 0)
      printf("%d ", low);

    ++low;
  }

  return 0;
}

int main()
{
  int low, high, i, flag, temp;
  printf("Enter two numbers(intevals): ");
  scanf("%d %d", &low, &high);

  //swapping numbers if low is greater than high
  if (low > high) {
    temp = low;
    low = high;
    high = temp;
  }

  printf("Prime numbers between %d and %d are: ", low, high);

  while (low < high)
  {
    flag = 0;

    for(i = 2; i <= low/2; ++i)
    {
      if(low % i == 0)
      {
        flag = 1;
        break;
      }
    }

    if (flag == 0)
      printf("%d ", low);

    ++low;
  }

  return 0;
}

int main()
{
  int n1, n2, i, gcd;

  printf("Enter two integers: ");
  scanf("%d %d", &n1, &n2);

  for(i=1; i <= n1 && i <= n2; ++i)
  {
    // Checks if i is factor of both integers
    if(n1%i==0 && n2%i==0)
      gcd = i;
  }

  printf("G.C.D of %d and %d is %d", n1, n2, gcd);

  return 0;
}

int main()
{
  int n1, n2;

  printf("Enter two positive integers: ");
  scanf("%d %d",&n1,&n2);

  while(n1!=n2)
  {
    if(n1 > n2)
      n1 -= n2;
    else
      n2 -= n1;
  }
  printf("GCD = %d",n1);

  return 0;
}

int main()
{
  int n1, n2;

  printf("Enter two integers: ");
  scanf("%d %d",&n1,&n2);

  // if user enters negative number, sign of the number is changed to positive
  n1 = ( n1 > 0) ? n1 : -n1;
  n2 = ( n2 > 0) ? n2 : -n2;

  while(n1!=n2)
  {
    if(n1 > n2)
      n1 -= n2;
    else
      n2 -= n1;
  }
  printf("GCD = %d",n1);

  return 0;
}

enum {
  /*
   * worker_pool flags
   *
   * A bound pool is either associated or disassociated with its CPU.
   * While associated (!DISASSOCIATED), all workers are bound to the
   * CPU and none has %WORKER_UNBOUND set and concurrency management
   * is in effect.
   *
   * While DISASSOCIATED, the cpu may be offline and all workers have
   * %WORKER_UNBOUND set and concurrency management disabled, and may
   * be executing on any CPU.  The pool behaves as an unbound one.
   *
   * Note that DISASSOCIATED should be flipped only while holding
   * wq_pool_attach_mutex to avoid changing binding state while
   * worker_attach_to_pool() is in progress.
   */
      POOL_MANAGER_ACTIVE	= 1 << 0,	/* being managed */
  POOL_DISASSOCIATED	= 1 << 2,	/* cpu can't serve workers */

  /* worker flags */
      WORKER_DIE		= 1 << 1,	/* die die die */
  WORKER_IDLE		= 1 << 2,	/* is idle */
  WORKER_PREP		= 1 << 3,	/* preparing to run works */
  WORKER_CPU_INTENSIVE	= 1 << 6,	/* cpu intensive */
  WORKER_UNBOUND		= 1 << 7,	/* worker is unbound */
  WORKER_REBOUND		= 1 << 8,	/* worker was rebound */

  WORKER_NOT_RUNNING	= WORKER_PREP | WORKER_CPU_INTENSIVE |
      WORKER_UNBOUND | WORKER_REBOUND,

  NR_STD_WORKER_POOLS	= 2,		/* # standard pools per cpu */

  UNBOUND_POOL_HASH_ORDER	= 6,		/* hashed by pool->attrs */
  BUSY_WORKER_HASH_ORDER	= 6,		/* 64 pointers */

  MAX_IDLE_WORKERS_RATIO	= 4,		/* 1/4 of busy can be idle */
  IDLE_WORKER_TIMEOUT	= 300 * HZ,	/* keep idle ones for 5 mins */

  MAYDAY_INITIAL_TIMEOUT  = HZ / 100 >= 2 ? HZ / 100 : 2,
  /* call for help after 10ms
     (min two ticks) */
      MAYDAY_INTERVAL		= HZ / 10,	/* and then every 100ms */
  CREATE_COOLDOWN		= HZ,		/* time to breath after fail */

  /*
   * Rescue workers are used only on emergencies and shared by
   * all cpus.  Give MIN_NICE.
   */
      RESCUER_NICE_LEVEL	= MIN_NICE,
  HIGHPRI_NICE_LEVEL	= MIN_NICE,

  WQ_NAME_LEN		= 24,
};

/*
 * Structure fields follow one of the following exclusion rules.
 *
 * I: Modifiable by initialization/destruction paths and read-only for
 *    everyone else.
 *
 * P: Preemption protected.  Disabling preemption is enough and should
 *    only be modified and accessed from the local cpu.
 *
 * L: pool->lock protected.  Access with pool->lock held.
 *
 * X: During normal operation, modification requires pool->lock and should
 *    be done only from local cpu.  Either disabling preemption on local
 *    cpu or grabbing pool->lock is enough for read access.  If
 *    POOL_DISASSOCIATED is set, it's identical to L.
 *
 * A: wq_pool_attach_mutex protected.
 *
 * PL: wq_pool_mutex protected.
 *
 * PR: wq_pool_mutex protected for writes.  Sched-RCU protected for reads.
 *
 * PW: wq_pool_mutex and wq->mutex protected for writes.  Either for reads.
 *
 * PWR: wq_pool_mutex and wq->mutex protected for writes.  Either or
 *      sched-RCU for reads.
 *
 * WQ: wq->mutex protected.
 *
 * WR: wq->mutex protected for writes.  Sched-RCU protected for reads.
 *
 * MD: wq_mayday_lock protected.
 */

/* struct worker is defined in workqueue_internal.h */

struct worker_pool {
  spinlock_t		lock;		/* the pool lock */
  int			cpu;		/* I: the associated cpu */
  int			node;		/* I: the associated node ID */
  int			id;		/* I: pool ID */
  unsigned int		flags;		/* X: flags */

  unsigned long		watchdog_ts;	/* L: watchdog timestamp */

  struct list_head	worklist;	/* L: list of pending works */

  int			nr_workers;	/* L: total number of workers */
  int			nr_idle;	/* L: currently idle workers */

  struct list_head	idle_list;	/* X: list of idle workers */
  struct timer_list	idle_timer;	/* L: worker idle timeout */
  struct timer_list	mayday_timer;	/* L: SOS timer for workers */

  /* a workers is either on busy_hash or idle_list, or the manager */
  DECLARE_HASHTABLE(busy_hash, BUSY_WORKER_HASH_ORDER);
  /* L: hash of busy workers */

  struct worker		*manager;	/* L: purely informational */
  struct list_head	workers;	/* A: attached workers */
  struct completion	*detach_completion; /* all workers detached */

  struct ida		worker_ida;	/* worker IDs for task name */

  struct workqueue_attrs	*attrs;		/* I: worker attributes */
  struct hlist_node	hash_node;	/* PL: unbound_pool_hash node */
  int			refcnt;		/* PL: refcnt for unbound pools */

  /*
   * The current concurrency level.  As it's likely to be accessed
   * from other CPUs during try_to_wake_up(), put it in a separate
   * cacheline.
   */
  atomic_t		nr_running ____cacheline_aligned_in_smp;

  /*
   * Destruction of pool is sched-RCU protected to allow dereferences
   * from get_work_pool().
   */
  struct rcu_head		rcu;
} ____cacheline_aligned_in_smp;

/*
 * The per-pool workqueue.  While queued, the lower WORK_STRUCT_FLAG_BITS
 * of work_struct->data are used for flags and the remaining high bits
 * point to the pwq; thus, pwqs need to be aligned at two's power of the
 * number of flag bits.
 */
struct pool_workqueue {
  struct worker_pool	*pool;		/* I: the associated pool */
  struct workqueue_struct *wq;		/* I: the owning workqueue */
  int			work_color;	/* L: current color */
  int			flush_color;	/* L: flushing color */
  int			refcnt;		/* L: reference count */
  int			nr_in_flight[WORK_NR_COLORS];
  /* L: nr of in_flight works */
  int			nr_active;	/* L: nr of active works */
  int			max_active;	/* L: max active works */
  struct list_head	delayed_works;	/* L: delayed works */
  struct list_head	pwqs_node;	/* WR: node on wq->pwqs */
  struct list_head	mayday_node;	/* MD: node on wq->maydays */

  /*
   * Release of unbound pwq is punted to system_wq.  See put_pwq()
   * and pwq_unbound_release_workfn() for details.  pool_workqueue
   * itself is also sched-RCU protected so that the first pwq can be
   * determined without grabbing wq->mutex.
   */
  struct work_struct	unbound_release_work;
  struct rcu_head		rcu;
} __aligned(1 << WORK_STRUCT_FLAG_BITS);

/*
 * Structure used to wait for workqueue flush.
 */
struct wq_flusher {
  struct list_head	list;		/* WQ: list of flushers */
  int			flush_color;	/* WQ: flush color waiting for */
  struct completion	done;		/* flush completion */
};

struct wq_device;
