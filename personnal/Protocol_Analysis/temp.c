int NSCLASS dsr_rreq_route_discovery(struct in_addr target)
{
	struct rreq_tbl_entry *e;
	int ttl, res = 0;
	struct timeval expires;

#define	TTL_START 1

	DSR_WRITE_LOCK(&rreq_tbl.lock);

	e = (struct rreq_tbl_entry *)__tbl_find(&rreq_tbl, &target, crit_addr);

	if (!e)
		e = __rreq_tbl_add(target);
	else {
		/* Put it last in the table */
		__tbl_detach(&rreq_tbl, &e->l);
		__tbl_add_tail(&rreq_tbl, &e->l);
	}

	if (!e) {
		res = -ENOMEM;
		goto out;
	}

	if (e->state == STATE_IN_ROUTE_DISC) {
		DEBUG("Route discovery for %s already in progress\n",
		      print_ip(target));
		goto out;
	}
	DEBUG("Route discovery for %s\n", print_ip(target));

	gettime(&e->last_used);
	e->ttl = ttl = TTL_START;
	/* The draft does not actually specify how these Request Timeout values
	 * should be used... ??? I am just guessing here. */

	if (e->ttl == 1)
		e->timeout = ConfValToUsecs(NonpropRequestTimeout);
	else
		e->timeout = ConfValToUsecs(RequestPeriod);

	e->state = STATE_IN_ROUTE_DISC;
	e->num_rexmts = 0;

	expires = e->last_used;
	timeval_add_usecs(&expires, e->timeout);

	set_timer(e->timer, &expires);

	DSR_WRITE_UNLOCK(&rreq_tbl.lock);

	dsr_rreq_send(target, ttl);

	return 1;
      out:
	DSR_WRITE_UNLOCK(&rreq_tbl.lock);

	return res;
}
